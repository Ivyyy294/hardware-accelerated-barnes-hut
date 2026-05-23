#include "CSUtils.hlsli"
#include "CSStack.hlsli"

#define NUM_THREADS 256
#define L 10

cbuffer SettingsBuffer : register(b0)
{
    uint pCount;
    float globallCellWidth;
    float ugc;
    float theta;
}

//Particle data buffer
StructuredBuffer<uint> keyBuffer : register(t0);
StructuredBuffer<float3> posBuffer : register(t1);
StructuredBuffer<float> massBuffer : register(t2);
StructuredBuffer<uint> newToOldIndexBuffer : register(t3);

//Tree data buffer
StructuredBuffer<uint> kBuffer : register(t4);
StructuredBuffer<uint> nkBuffer : register(t5);
StructuredBuffer<uint> loBuffer : register(t6);
StructuredBuffer<uint> coBuffer : register(t7);
StructuredBuffer<uint> counterBuffer : register(t8);

//Multipole data
StructuredBuffer<float3> centerOfMass : register(t9);
StructuredBuffer<float> combinedMass : register(t10);

StructuredBuffer<uint> liBuffer : register(t11);
StructuredBuffer<uint> lcBuffer : register(t12);

RWStructuredBuffer<float3> forceBuffer : register(u0);

//Groupshared
groupshared CStack stack;

groupshared uint nodeIndex = 0;
groupshared float criterium = 0.f;
groupshared uint minSqrDistInt = 0;
groupshared bool gDone = false;

groupshared uint nodeStack[256];
groupshared uint particleStack[256];
groupshared int particleStackIndex = 0;
groupshared int nodeStackIndex = 0;

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
    uint firstChild;
    bool isInternalNode;
    float cellWidth;
    float3 com;
    float mass;
    float3 direction;
    float sqrDistance;
};

void GetParticleData (const uint index, bool isParticle, out ParticleData data)
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

void GetNodeData(out NodeData data, in const uint nodeIndex, float3 pos)
{
    const uint nk = nkBuffer[nodeIndex];
    const uint level = GetTreeLevelFromWs(nk);
    data.firstChild = coBuffer[nodeIndex];
    data.isInternalNode = data.firstChild > 0;
    data.cellWidth = globallCellWidth / (1 << level);
    data.com = centerOfMass[nodeIndex];
    data.mass = combinedMass[nodeIndex];
    data.direction = data.com - pos;
        
        //Find smallest distance
    data.sqrDistance = dot(data.direction, data.direction);
}

float3 ApplyForceFromNode(float pMass, float p2Mass, float3 direction, float sqrDistance)
{
    if (sqrDistance < 0.001f)
        return float3(0, 0, 0);
    
    float forceMag = ugc * ((p2Mass * pMass) / sqrDistance);
    
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

bool IsTraversalDone(const uint3 GTid, out int localNodeIndex)
{
    if (GTid.x == 0)
    {
        gDone = stack.IsEmpty();
        
        if (!gDone)
            stack.Pop(nodeIndex);
    }
        
    GroupMemoryBarrierWithGroupSync();
    bool done = gDone;
    localNodeIndex = nodeIndex;
        
    return done;
}

//void ApplyNodeStack(inout ParticleData pData, in uint stack[256], const uint count)
//{
//    for (uint i = 0; i < count; ++i)
//    {
//        const float nodeMass = combinedMass[stack[i]];
//        pData.force += ApplyForceFromNode(pData.mass, nodeMass, direction, sqrDistance);
//    }
    
//    GroupMemoryBarrierWithGroupSync();
//}

//void ApplyParticleStack(inout ParticleData pData, in uint stack[256], const uint count)
//{
//    for (uint i = 0; i < count; ++i)
//        pData.force += ApplyCloseRangeForces(pData.mass, pData.pos, stack[i]);
    
//    GroupMemoryBarrierWithGroupSync();
//}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint ni = counterBuffer[1];
    const uint nl = counterBuffer[0];
    const uint n = ni + nl;
    const bool gIsInternalNode = coBuffer[Gid.x] > 0;
    
    //For every leaf node, therminate internal node groups
    if (gIsInternalNode > 0)
        return;
    
    const uint gPCount = lcBuffer[Gid.x];
    const uint gFirstChild = liBuffer[Gid.x];
    const uint index = gFirstChild + GTid.x;
    
    ParticleData pData;
    
    GetParticleData(index, GTid.x <= gPCount, pData);

    if (GTid.x == 0)
    {
        stack.Initialize();
        stack.Push(0);
    }
    
    [loop]
    while (true)
    {
        int localNodeIndex = -1;
        
        if (IsTraversalDone(GTid, localNodeIndex))
            break;

        NodeData nData;
        
        GetNodeData(nData, localNodeIndex, pData.pos);

        if (pData.isParticle)
            InterlockedMin(minSqrDistInt, FloatToUint(nData.sqrDistance));

        GroupMemoryBarrierWithGroupSync();
            
        float minSqrDist = UintToFloat(minSqrDistInt);
            
        if (GTid.x == 0)
            criterium = minSqrDist > 0.f ? (nData.cellWidth * nData.cellWidth) / minSqrDist : theta;
            
        GroupMemoryBarrierWithGroupSync();
            
        float localCriterium = criterium;
        
        //apply node
        if (localCriterium < theta)
        {
            const float nodeMass = combinedMass[localNodeIndex];
            pData.force += ApplyForceFromNode(pData.mass, nodeMass, nData.direction, nData.sqrDistance);
        }
        //push childen
        else if (nData.isInternalNode)
        {
            if (GTid.x == 0)
            {
                const uint count = 8;
            
                for (uint i = 0; i < count; ++i)
                    stack.Push(nData.firstChild + i);
            }
        }
        //apply leafs
        else
            pData.force += ApplyCloseRangeForces(pData.mass, pData.pos, localNodeIndex);
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    if (pData.isParticle)
        forceBuffer[pData.destIndex] = pData.force;
}