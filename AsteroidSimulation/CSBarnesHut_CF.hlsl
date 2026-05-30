#include "CSUtils.hlsli"
#include "CSDoubleBuffer.hlsli"

#define NUM_THREADS 256
#define NUM_PARALLEL_NODES 8
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
    uint index;
    uint firstChild;
    bool isInternalNode;
    float cellWidth;
    float3 com;
    float mass;
    float3 center;
    bool criterium;
    int minSqrDistInt;
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
groupshared CDoubleBuffer stack;

groupshared bool gDone = false;

groupshared bool gIsInternalNode = false;
groupshared uint gPCount = 0;
groupshared uint gFirstChild = 0;

groupshared NodeData gNodeData[NUM_PARALLEL_NODES];
groupshared uint gBaseNodeIndex = 0;

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

void GetNodeData(out NodeData data, in const uint nodeIndex)
{
    const uint nk = nkBuffer[nodeIndex];
    const uint level = GetTreeLevelFromWs(nk);
    data.index = nodeIndex;
    data.firstChild = coBuffer[nodeIndex];
    data.isInternalNode = data.firstChild > 0;
    data.cellWidth = cellWidth[ nodeIndex];
    data.com = centerOfMass[nodeIndex];
    data.mass = combinedMass[nodeIndex];
    data.center = cellCenter[nodeIndex];
    data.criterium = false;
    data.minSqrDistInt = 0xFFFFFFFF;
    
    debugBuffer[nodeIndex] = data.cellWidth;
}

float3 ApplyForceFromNode(float pMass, const float3 pPos, NodeData nData)
{
    float3 direction = nData.com - pPos;
    float sqrDistance = dot(direction, direction);
    
    if (sqrDistance < 0.001f)
        return float3(0, 0, 0);
    
    float forceMag = ugc * ((nData.mass * pMass) / sqrDistance);
    
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
        gDone = stack.IsFrontEmpty() && stack.IsBackEmpty();
        
        if (!gDone)
        {
            if (stack.IsFrontEmpty())
                stack.Swap();
        }
    }
        
    GroupMemoryBarrierWithGroupSync();
    bool done = gDone;
        
    return done;
}

void SetCriteriumForNode(const uint3 GTid)
{
    const uint nodeDataIndex = GTid.x;

    float minSqrDist = UintToFloat(gNodeData[nodeDataIndex].minSqrDistInt);
    float delta = distance(gNodeData[nodeDataIndex].com, gNodeData[nodeDataIndex].center);
    float critVal = (gNodeData[nodeDataIndex].cellWidth / theta) + delta;
    gNodeData[nodeDataIndex].criterium = minSqrDist > (critVal * critVal);
}

void SetMinDistForNode(const uint nodeDataIndex, const ParticleData pData)
{
    float3 direction = gNodeData[nodeDataIndex].com - pData.pos;
    float sqrDistance = dot(direction, direction);
        
    GroupMemoryBarrierWithGroupSync();
        
    if (pData.isParticle)
        InterlockedMin(gNodeData[nodeDataIndex].minSqrDistInt, FloatToUint(sqrDistance));
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
        stack.Initialize();
        stack.Push(1);
    }
    
    [loop]
    while (true)
    {
        if (IsTraversalDone(GTid))
            break;

        if (GTid.x == 0)
            stack.Pop(gBaseNodeIndex);
        
        GroupMemoryBarrierWithGroupSync();

        //Load Node Data
        if (GTid.x < NUM_PARALLEL_NODES)
        {
            int localNodeIndex = gBaseNodeIndex + GTid.x;
            GetNodeData(gNodeData[GTid.x], localNodeIndex);
        }
        
        GroupMemoryBarrierWithGroupSync();
        
        //Determin minDist
        for (uint iDist = 0; iDist < NUM_PARALLEL_NODES; ++iDist)
            SetMinDistForNode(iDist, pData);
        
        GroupMemoryBarrierWithGroupSync();
        
        //Determin criterium
        if (GTid.x < NUM_PARALLEL_NODES)
        {
            SetCriteriumForNode(GTid);
            
            if (gNodeData[GTid.x].criterium == false && gNodeData[GTid.x].isInternalNode)
                stack.InterlockedPush(GTid.x, gNodeData[GTid.x].firstChild);
        }
        
        GroupMemoryBarrierWithGroupSync();
        
        //Apply nodes
        for (uint j = 0; j < NUM_PARALLEL_NODES; ++j)
        {
            NodeData nData = gNodeData[j];
        
            //apply node
            if (nData.criterium)
                pData.force += ApplyForceFromNode(pData.mass, pData.pos, nData);
            else if (nData.isInternalNode == false)
                pData.force += ApplyCloseRangeForces(pData.mass, pData.pos, nData.index);
        }
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    if (pData.isParticle)
        forceBuffer[pData.destIndex] = pData.force;
}