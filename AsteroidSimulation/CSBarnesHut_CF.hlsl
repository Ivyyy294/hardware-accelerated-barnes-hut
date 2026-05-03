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

RWStructuredBuffer<float3> forceBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u7);


[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    const uint ni = counterBuffer[1];
    const uint maxLevel = GetTreeLevelFromWs(nkBuffer[ni]);
    
    if (index >= pCount)
        return;

    const float3 pPos = posBuffer[index];
    const float pMass = massBuffer[index];
    const uint pDestIndex = newToOldIndexBuffer[index];
    float3 pForce = {0.f, 0.f, 0.f};
    
    //pForce = -pPos * 0.1f;
    //forceBuffer[pDestIndex] = pForce;
    //return;
    
    uint stack[64];
    int current = 0;
    
    stack[0] = 0;

    while (current >= 0)
    {
        const uint nodeIndex = stack[current--];
        const uint nk = nkBuffer[nodeIndex];
        const uint level = GetTreeLevelFromWs(nk);
        const bool isLeaf = nodeIndex > ni;
        
        const float cellWidth = globallCellWidth / (level + 1);
        const float3 nodeCoM = centerOfMass[nodeIndex];
        float3 direction = nodeCoM - pPos;
        float sqrDistance = dot(direction, direction);
        float criterium = (cellWidth * cellWidth) / sqrDistance;
        
        //apply 
        if (criterium < theta && sqrDistance > 0.f)
        {
            const float nodeMass = combinedMass[nodeIndex];
            const float force = ugc * ((nodeMass * pMass) / sqrDistance);
            const float3 nDirection = normalize(direction);
            pForce += nDirection * force;
        }
        //apply child particle
        else if (isLeaf)
        {
            const uint kVal = kBuffer[nodeIndex];
            const uint k1Val = kBuffer[nodeIndex + 1];
        
            uint indexBegin = BinarySearch(kVal, keyBuffer, pCount);
            uint indexLast = BinarySearch(k1Val, keyBuffer, pCount);
        
            for (uint i = indexBegin; i < indexLast; ++i)
            {
                const float p2Mass = massBuffer[i];
                const float3 p2Pos = posBuffer[i];
                float3 direction = p2Pos - pPos;
                const float p2SqrDistance = dot(direction, direction);
                
                if (p2SqrDistance > 0.f)
                {
                    direction = normalize(direction);
                    const float force = ugc * ((p2Mass * pMass) / p2SqrDistance);
                    pForce += direction * force;
                }
            }
        }
        //push childen
        else
        {
            const uint firstChild = coBuffer[nodeIndex];
            
            for (uint i = 0; i < 8; ++i)
                stack[++current] = firstChild + i;
        }
    }
    
    forceBuffer[pDestIndex] = pForce;
}