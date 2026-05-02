#include "CSUtils.hlsli"

#define L 10
#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint pCount;
    float globallCellWidth;
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

void CallculateLeafNodes (const uint index, const uint nl, const uint pCount)
{
    for (int i = 0; i < nl; i += NUM_THREADS)
    {
        uint nlIndex = i + index;
        
        if (nlIndex < nl)
        {
            float mass = 0.f;
            float3 center;
    
            const uint kVal = kBuffer[index];
            const uint k1Val = kBuffer[index + 1];
        
            uint indexBegin = BinarySearch(kVal, keyBuffer, pCount);
            uint indexLast = BinarySearch(k1Val, keyBuffer, pCount);
        
            for (uint i = indexBegin; i < indexLast; ++i)
            {
                const float iMass = massBuffer[i];
                mass += iMass;
        
                const float3 iPos = posBuffer[i];
                center.x += iPos.x * iMass;
                center.y += iPos.y * iMass;
                center.z += iPos.z * iMass;
            }
    
            centerOfMass[nlIndex] = center / mass;
            combinedMass[nlIndex] = mass;
            debugBuffer[nlIndex] = combinedMass[nlIndex];
        }
    }
}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    const uint ni = counterBuffer[1];
    const uint nl = counterBuffer[0];
    const uint n = ni + nl;
    
    CallculateLeafNodes(index, nl, pCount);
    
    GroupMemoryBarrierWithGroupSync();
    //debugBuffer[index] = pCount;
    //debugBuffer[index] = kBuffer[index];
    //debugBuffer[index] = nkBuffer[index];
    //debugBuffer[index] = coBuffer[index];
    return;
    //return;
    
    
    for (int i = 0; i < nl; i += NUM_THREADS)
    {
    }
    
    const uint indexK = index + ni;
    //Init leaf nodes K
        if (indexK >= nl)
            return;

    combinedMass[index] = -1.f;
    
    float mass = 0.f;
    float3 center;
    
    const uint kVal = kBuffer[index];
    const uint k1Val = kBuffer[index + 1];
        
    uint indexBegin = BinarySearch(kVal, keyBuffer, 65);
    uint indexLast = BinarySearch(k1Val, keyBuffer, 65);
        
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
    debugBuffer[indexK] = combinedMass[indexK];
    GroupMemoryBarrierWithGroupSync();
    return;
    if (index >= ni)
        return;

    mass = 0.f;
    center = float3(0.f, 0.f, 0.f);
    
    indexBegin = coBuffer[index];
    
    //debugBuffer[index] = indexBegin;
    return;
    
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
    //debugBuffer[index] = combinedMass[index];
}