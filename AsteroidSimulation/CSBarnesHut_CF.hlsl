#include "CSUtils.hlsli"

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

groupshared uint next[512];
groupshared uint current[512];

groupshared int currentIndex = 0;
groupshared int nextIndex = 0;

groupshared int nodeIndex = 0;
groupshared float criterium = 0.f;
groupshared uint minSqrDistInt = 0;

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
    
    float3 pPos = 0.f;
    float pMass = 0.f;
    uint pDestIndex = 0;
    float3 pForce = 0.f;
    const bool isParticle = GTid.x <= gPCount;
    
    //For gPCount threads
    if (isParticle)
    {
        pPos = posBuffer[index];
        pMass = massBuffer[index];
        pDestIndex = newToOldIndexBuffer[index];
    }
    
    if (GTid.x == 0)
    {
        current[0] = 0;
        currentIndex = 0;
    }
    
    [loop]
    while (true)
    {
        if (GTid.x == 0)
        {
            if (currentIndex < 0 && nextIndex < 0)
                nodeIndex = -1;
            else if (currentIndex < 0)
            {
                current = next;
                currentIndex = nextIndex;
                nextIndex = -1;
                nodeIndex = current[currentIndex--];
            }
            else
                nodeIndex = current[currentIndex--];
        }
        
        GroupMemoryBarrierWithGroupSync();
        
        int localNodeIndex = nodeIndex;
        
        if (localNodeIndex < 0)
            break;

        const uint nk = nkBuffer[localNodeIndex];
        const uint level = GetTreeLevelFromWs(nk);
        const uint firstChild = coBuffer[localNodeIndex];
        const bool isInternalNode = firstChild > 0;
        const float cellWidth = globallCellWidth / (1 << level);
        const float3 nodeCoM = centerOfMass[localNodeIndex];
        
        float3 direction = nodeCoM - pPos;
        
        //Find smallest distance
        float sqrDistance = dot(direction, direction);

        if (isParticle)
            InterlockedMin(minSqrDistInt, FloatToUint(sqrDistance));

        GroupMemoryBarrierWithGroupSync();
            
        float minSqrDist = UintToFloat(minSqrDistInt);
            
        if (GTid.x == 0)
            criterium = minSqrDist > 0.f ? (cellWidth * cellWidth) / minSqrDist : theta;
            
        GroupMemoryBarrierWithGroupSync();
            
        float localCriterium = criterium;
        
        //apply node
        if (localCriterium < theta)
        {
            const float nodeMass = combinedMass[localNodeIndex];
            pForce += ApplyForceFromNode(pMass, nodeMass, direction, minSqrDist);
        }
        //push childen
        else if (isInternalNode)
        {
            if (GTid.x == 0)
            {
                const uint count = 8;
            
                for (uint i = 0; i < count; ++i)
                    next[++nextIndex] = firstChild + i;
            }
            //GroupMemoryBarrierWithGroupSync();
        }
        //apply leafs
        else
            pForce += ApplyCloseRangeForces(pMass, pPos, localNodeIndex);
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    if (isParticle)
        forceBuffer[pDestIndex] = pForce;
}