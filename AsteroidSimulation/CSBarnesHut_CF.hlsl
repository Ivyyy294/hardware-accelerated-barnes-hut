#include "CSUtils.hlsli"
#include "CSDoubleBuffer.hlsli"

#define NUM_THREADS 64
#define L 10

cbuffer SettingsBuffer : register(b0)
{
    uint pCount;
    float globallCellWidth;
    float ugc;
    float theta;
}

struct ParticleData
{
    float3 pos;
    float mass;
    uint destIndex;
    float3 force;
    bool isParticle;
};

struct NodeData
{
    uint index[NUM_THREADS];
    uint firstChild[NUM_THREADS];
    bool isInternalNode[NUM_THREADS];
    float cellWidth[NUM_THREADS];
    float3 com[NUM_THREADS];
    float mass[NUM_THREADS];
    float3 center[NUM_THREADS];
    bool criterium[NUM_THREADS];
    int minSqrDistInt[NUM_THREADS];
};

//Particle data buffer
StructuredBuffer<float3> posBuffer : register(t0);
StructuredBuffer<float> massBuffer : register(t1);
StructuredBuffer<uint> newToOldIndexBuffer : register(t2);

//Tree data buffer
StructuredBuffer<uint> nkBuffer : register(t3);
StructuredBuffer<uint> coBuffer : register(t4);

//Multipole data
StructuredBuffer<float3> centerOfMass : register(t5);
StructuredBuffer<float> combinedMass : register(t6);
StructuredBuffer<uint> liBuffer : register(t7);
StructuredBuffer<uint> lcBuffer : register(t8);
StructuredBuffer<float> cellWidth : register(t9);
StructuredBuffer<float3> cellCenter : register(t10);

RWStructuredBuffer<float3> forceBuffer : register(u0);

//Groupshared
groupshared bool gDone = false;

groupshared bool gIsInternalNode = false;
groupshared uint gPCount = 0;
groupshared uint gFirstChild = 0;

groupshared NodeData gNodeData;
groupshared uint gNodeCount;

void GetParticleData(const uint index, bool isParticle, out ParticleData data)
{
    data.isParticle = isParticle;
    
    if (data.isParticle)
    {
        data.pos = posBuffer[index];
        data.mass = massBuffer[index];
        data.destIndex = newToOldIndexBuffer[index];
        data.force = 0.f;
    }
    else
    {
        data.pos = 0.f;
        data.mass = 0.f;
        data.destIndex = 0;
        data.force = 0.f;
    }
}

void GetNodeData(in const uint slot, in const uint nodeIndex)
{
    const uint nk = nkBuffer[nodeIndex];
    const uint level = GetTreeLevelFromWs(nk);
    gNodeData.index[slot] = nodeIndex;
    gNodeData.firstChild[slot] = coBuffer[nodeIndex];
    gNodeData.isInternalNode[slot] = coBuffer[nodeIndex] > 0;
    gNodeData.cellWidth[slot] = cellWidth[nodeIndex];
    gNodeData.com[slot] = centerOfMass[nodeIndex];
    gNodeData.mass[slot] = combinedMass[nodeIndex];
    gNodeData.center[slot] = cellCenter[nodeIndex];
    gNodeData.criterium[slot] = false;
    gNodeData.minSqrDistInt[slot] = 0xFFFFFFFF;
}

float3 ApplyForceFromNode(float pMass, const float3 pPos, const uint nodeIndex)
{
    float3 direction = gNodeData.com[nodeIndex] - pPos;
    float sqrDistance = dot(direction, direction);
    
    if (sqrDistance < 0.001f)
        return float3(0, 0, 0);
    
    float forceMag = ugc * ((gNodeData.mass[nodeIndex] * pMass) / sqrDistance);
    
    const float3 forceDirection = normalize(direction);
    
    return forceDirection * forceMag;
}

float3 GetForceFromParticle(const float pMass, const float3 pPos, uint p2Index)
{
    const float p2Mass = massBuffer[p2Index];
    const float3 p2Pos = posBuffer[p2Index];
    float3 forceDirection = p2Pos - pPos;
    const float sqrDistance = dot(forceDirection, forceDirection);
    
    if (sqrDistance < 0.001f)
        return float3(0, 0, 0);
    
    forceDirection = normalize(forceDirection);
    float forceMag = ugc * ((p2Mass * pMass) / sqrDistance);
    
    return forceDirection * forceMag;
}

float3 ApplyCloseRangeForces(const float pMass, const float3 pPos, const uint nodeIndex)
{
    float3 force = float3(0.f, 0.f, 0.f);
        
    uint indexBegin = liBuffer[nodeIndex];
    uint count = lcBuffer[nodeIndex];
        
    for (uint i = 0; i < count; ++i)
    {
        const uint p2Index = indexBegin + i;
        force += GetForceFromParticle(pMass, pPos, p2Index);
    }
    return force;
}

bool IsTraversalDone(const uint3 GTid)
{
    if (GTid.x == 0)
    {
        gDone = DB_IsFrontEmpty() && DB_IsBackEmpty();
        
        if (!gDone)
        {
            if (DB_IsFrontEmpty())
                DB_Swap();
        }
    }
        
    GroupMemoryBarrierWithGroupSync();
    bool done = gDone;
        
    return done;
}

void SetCriteriumForNode(const uint nodeDataIndex)
{
    float minSqrDist = UintToFloat(gNodeData.minSqrDistInt[nodeDataIndex]);
    float delta = distance(gNodeData.com[nodeDataIndex], gNodeData.center[nodeDataIndex]);
    float critVal = (gNodeData.cellWidth[nodeDataIndex] / theta) + delta;
    gNodeData.criterium[nodeDataIndex] = minSqrDist > (critVal * critVal);
}

void SetMinDistForNode(const uint nodeDataIndex, const ParticleData pData)
{
    float3 direction = gNodeData.com[nodeDataIndex] - pData.pos;
    float sqrDistance = dot(direction, direction);
        
    GroupMemoryBarrierWithGroupSync();
        
    if (pData.isParticle)
        InterlockedMin(gNodeData.minSqrDistInt[nodeDataIndex], FloatToUint(sqrDistance));
}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    if (GTid.x == 0)
    {
        gIsInternalNode = coBuffer[Gid.x] > 0;
        gPCount = lcBuffer[Gid.x];
        gFirstChild = liBuffer[Gid.x];
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    //For every leaf node, therminate internal node groups
    if (gIsInternalNode > 0)
        return;
    
    const uint index = gFirstChild + GTid.x;
    
    ParticleData pData;
    
    GetParticleData(index, GTid.x <= gPCount, pData);

    if (GTid.x == 0)
    {
        DB_Initialize();
        DB_Push(1);
        DB_Push(2);
        DB_Push(3);
        DB_Push(4);
        DB_Push(5);
        DB_Push(6);
        DB_Push(7);
        DB_Push(8);
    }
    
    [loop]
    while (true)
    {
        if (IsTraversalDone(GTid))
            break;

        if (GTid.x == 0)
            gNodeCount = 0;
        
        GroupMemoryBarrierWithGroupSync();
        
        uint nodeIndex;
        bool ok = DB_InterlockedPop(nodeIndex);
        uint slot = 0;
        
        if (ok)
        {
            InterlockedAdd(gNodeCount, 1, slot);
            GetNodeData(slot, nodeIndex);
        }
        
        GroupMemoryBarrierWithGroupSync();
        
        //Determin minDist
        const uint nodeCount = gNodeCount;
        
        for (uint iDist = 0; iDist < nodeCount; ++iDist)
            SetMinDistForNode(iDist, pData);
        
        GroupMemoryBarrierWithGroupSync();
        
        //Determin criterium
        if (ok)
        {
            SetCriteriumForNode(slot);
                
            if (!gNodeData.criterium[slot] && gNodeData.isInternalNode[slot])
            {
                uint first = gNodeData.firstChild[slot];

                [unroll]
                for (uint c = 0; c < 8; ++c)
                    DB_InterlockedPush(first + c);
            }
        }
        
        GroupMemoryBarrierWithGroupSync();
        
        //Apply nodes

        for (uint j = 0; j < nodeCount; ++j)
        {
            //apply node
            if (gNodeData.criterium[j])
                pData.force += ApplyForceFromNode(pData.mass, pData.pos, j);
            else if (gNodeData.isInternalNode[j] == false)
                pData.force += ApplyCloseRangeForces(pData.mass, pData.pos, gNodeData.index[j]);
        }
    }
    
    if (pData.isParticle)
        forceBuffer[pData.destIndex] = pData.force;
}