#include "CSUtils.hlsli"

#define L 10
#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint Count;
    float CellWidth;
    uint sPadding[2];
}

//Particle data buffer
StructuredBuffer<uint> keyBuffer : register(t0);
StructuredBuffer<float3> posBuffer : register(t1);
StructuredBuffer<float> massBuffer : register(t2);

//Tree data buffer
StructuredBuffer<uint> kBuffer : register(t3);
StructuredBuffer<uint> nkBuffer : register(t4);
StructuredBuffer<uint> loBuffer : register(t5);
StructuredBuffer<uint> coBuffer : register(t6);
StructuredBuffer<uint> counterBuffer : register(t7);

//Multipole data
RWStructuredBuffer<float3> centerOfMass : register(u0);
RWStructuredBuffer<float> combinedMass : register(u1);
RWStructuredBuffer<float> cellWidth : register(u2);

RWStructuredBuffer<uint> debugBuffer : register(u7);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    
    const uint ni = counterBuffer[1] - 1;
    const uint nl = counterBuffer[0];
    
    const uint indexK = index + ni;
    
    //Init leaf nodes K
    if (indexK >= nl)
        return;

    combinedMass[index] = -1.f;
    
    float mass = 0.f;
    float3 center;
    
    const uint kVal = kBuffer[index];
    const uint k1Val = kBuffer[index + 1];
        
    uint indexBegin = BinarySearch(kVal, keyBuffer, 33);
    uint indexLast = BinarySearch(k1Val, keyBuffer, 33);
        
    //debugBuffer[index] = indexLast;
        
    for (uint i = indexBegin; i < indexLast; ++i)
    {
        const float iMass = massBuffer[i];
        mass += iMass;
        
        const float3 iPos = posBuffer[i];
        center.x += iPos.x * iMass;
        center.y += iPos.y * iMass;
        center.z += iPos.z * iMass;
    }
    
    centerOfMass[indexK] = center / mass;
    combinedMass[indexK] = mass;

    GroupMemoryBarrierWithGroupSync();
    
    if (index >= ni)
        return;

    mass = 0.f;
    center = float3(0.f, 0.f, 0.f);
    
    indexBegin = coBuffer[index];
    
    for (uint i = indexBegin; i < indexBegin + 8;)
    {
        if (combinedMass[i] >= 0.f)
        {
            const float iMass = combinedMass[i];
            mass += iMass;
        
            const float3 iPos = centerOfMass[i];
            center.x += iPos.x * iMass;
            center.y += iPos.y * iMass;
            center.z += iPos.z * iMass;
            debugBuffer[index] = mass;
            ++i;
        }
    }
    
    centerOfMass[index] = center / mass;
    combinedMass[index] = mass;
    debugBuffer[index] = combinedMass[index];
}