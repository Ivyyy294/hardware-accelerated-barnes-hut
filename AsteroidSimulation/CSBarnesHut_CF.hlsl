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
    float4 comMass[NUM_THREADS];
    uint index[NUM_THREADS];
    int minSqrDistInt[NUM_THREADS];
    bool criterium[NUM_THREADS];
};

struct NodeParticleData
{
    float4 posMass[NUM_THREADS];
    uint count;
    uint firstChild;
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
groupshared uint gNodeCount;

groupshared NodeData gNodeData;
groupshared NodeParticleData gNodeParticleData;

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
    gNodeData.comMass[slot].xyz = centerOfMass[nodeIndex];
    gNodeData.comMass[slot].w = combinedMass[nodeIndex];
    gNodeData.criterium[slot] = false;
    gNodeData.minSqrDistInt[slot] = 0xFFFFFFFF;
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
    uint nodeIndex = gNodeData.index[nodeDataIndex];
    
    float minSqrDist = UintToFloat(gNodeData.minSqrDistInt[nodeDataIndex]);
    float delta = distance(gNodeData.comMass[nodeDataIndex].xyz, cellCenter[nodeIndex]);
    float critVal = (cellWidth[nodeIndex] / theta) + delta;
    gNodeData.criterium[nodeDataIndex] = minSqrDist > (critVal * critVal);
}

void SetMinDistForNode(const uint nodeDataIndex, const ParticleData pData)
{
    float3 direction = gNodeData.comMass[nodeDataIndex].xyz - pData.pos;
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
            
            bool isInternalNode = coBuffer[gNodeData.index[slot]] > 0;
            
            if (!gNodeData.criterium[slot] && isInternalNode)
            {
                uint first = coBuffer[gNodeData.index[slot]];

                [unroll]
                for (uint c = 0; c < 8; ++c)
                    DB_InterlockedPush(first + c);
            }
        }
        
        GroupMemoryBarrier();
        
        //Apply nodes

        for (uint j = 0; j < nodeCount; ++j)
        {
            uint currentNodeIndex = gNodeData.index[j];
            bool isInternalNode = coBuffer[currentNodeIndex] > 0;
            
            //apply from node
            if (gNodeData.criterium[j])
            {
                float3 direction = gNodeData.comMass[j].xyz - pData.pos;
                float sqrDistance = dot(direction, direction);
                const float distance = sqrDistance * rsqrt(sqrDistance);
                
                if (sqrDistance >= 0.001f)
                    pData.force += (gNodeData.comMass[j].w * direction) / (sqrDistance * distance);
            }
            //apply particle of node
            else if (isInternalNode == false)
            {
                //Preload count and first particle index
                if (GTid.x == 0)
                {
                    gNodeParticleData.firstChild = liBuffer[currentNodeIndex];
                    gNodeParticleData.count = min(NUM_THREADS, lcBuffer[currentNodeIndex]);
                }
                
                GroupMemoryBarrierWithGroupSync();
                
                //preload particle mass and pos
                if (GTid.x < gNodeParticleData.count)
                {
                    uint pIndex = gNodeParticleData.firstChild + GTid.x;
                    gNodeParticleData.posMass[GTid.x].w = massBuffer[pIndex];
                    gNodeParticleData.posMass[GTid.x].xyz = posBuffer[pIndex];
                }
                
                GroupMemoryBarrierWithGroupSync();
                
                float3 tmpForce = 0.f;
                
                for (uint pIndex = 0; pIndex < gNodeParticleData.count; ++pIndex)
                {
                    float3 forceDirection = gNodeParticleData.posMass[pIndex].xyz - pData.pos;
                    const float sqrDistance = dot(forceDirection, forceDirection);
                    const float distance = sqrDistance * rsqrt(sqrDistance);
    
                    if (sqrDistance < 0.001f)
                        continue;
    
                    pData.force += (gNodeParticleData.posMass[pIndex].w * forceDirection) / (sqrDistance * distance);
                }
            }
        }
    }
    
    if (pData.isParticle)
        forceBuffer[pData.destIndex] = (ugc * pData.mass) * pData.force;
}