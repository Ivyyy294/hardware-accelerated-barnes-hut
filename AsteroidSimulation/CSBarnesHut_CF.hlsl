#include "CSUtils.hlsli"

#define NUM_THREADS 256
#define L 10
#define STACK_SIZE 512

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
RWStructuredBuffer<uint> debugBuffer : register(u7);

float3 ApplyForceFromNode(float pMass, float p2Mass, float3 direction, float sqrDistance)
{
    if (sqrDistance == 0.f)
        return float3(0, 0, 0);
    
    float forceMag = (ugc * p2Mass * pMass) / sqrDistance;
    
    const float3 forceDirection = normalize(direction);
    
    return forceDirection * forceMag;
}

float3 GetForceFromParticle(const float pMass, const float3 pPos, uint p2Index)
{
    const float p2Mass = massBuffer[p2Index];
    const float3 p2Pos = posBuffer[p2Index];
    float3 forceDirection = p2Pos - pPos;
    const float sqrDistance = dot(forceDirection, forceDirection);
    
    if (sqrDistance == 0.f)
        return float3(0, 0, 0);
    
    forceDirection = normalize(forceDirection);
    float forceMag = (ugc * p2Mass * pMass) / sqrDistance;
    
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

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint index = Gid.x * NUM_THREADS + GTid.x;

    if (index >= pCount)
        return;
    
    const uint ni = counterBuffer[1];
    const uint maxLevel = GetTreeLevelFromWs(nkBuffer[ni]);

    const float3 pPos = posBuffer[index];
    const float pMass = massBuffer[index];
    const uint pDestIndex = newToOldIndexBuffer[index];
    float3 pForce = {0.f, 0.f, 0.f};
    
    uint currentStack[STACK_SIZE];
    uint nextStack[STACK_SIZE];
    
    int currentStackIndex = 0;
    int nextStackIndex = 0;
    
    //pForce = ApplyCloseRangeForces(pMass, pPos, 0);
    //forceBuffer[pDestIndex] = pForce;
    //debugBuffer[index] = kBuffer[index];
    //debugBuffer[index] = lcBuffer[index];
    //debugBuffer[index] = coBuffer[index];
    //debugBuffer[index] = loBuffer[index];
    //debugBuffer[index] = liBuffer[index];
    //return;
    //debugBuffer[index] = combinedMass[index];
    //return;
    
    //debugBuffer[index] = nkBuffer[index];
    
    nextStack[0] = 0;
    
    [loop]
    while (nextStackIndex >= 0)
    {
        currentStack = nextStack;
        currentStackIndex = nextStackIndex;
        nextStackIndex = -1;
        
        [loop]
        while (currentStackIndex >= 0)
        {
            if (currentStackIndex >= STACK_SIZE)
                return;
            
            const uint nodeIndex = currentStack[currentStackIndex--];
            const uint nk = nkBuffer[nodeIndex];
            const uint level = GetTreeLevelFromWs(nk);
            const uint firstChild = coBuffer[nodeIndex];
            const bool isInternalNode = firstChild > 0;
        
            const float cellWidth = globallCellWidth / (1 << level);
        
            const float3 nodeCoM = centerOfMass[nodeIndex];
            const float nodeMass = combinedMass[nodeIndex];
        
            float3 direction = nodeCoM - pPos;
            float sqrDistance = dot(direction, direction);
        
            float criterium = sqrDistance > 0.f ? (cellWidth * cellWidth) / sqrDistance : theta;
        
            //apply node
            if (criterium < theta)
                pForce += ApplyForceFromNode(pMass, nodeMass, direction, sqrDistance);
            //push childen
            else if (isInternalNode)
            {
                const uint count = 8;
            
                for (uint i = 0; i < count; ++i)
                    nextStack[++nextStackIndex] = firstChild + i;
            }
            //apply leafs
            else
                pForce += ApplyCloseRangeForces(pMass, pPos, nodeIndex);
        }
    }

    
    forceBuffer[pDestIndex] = pForce;
    //debugBuffer[index] = combinedMass[index];
}