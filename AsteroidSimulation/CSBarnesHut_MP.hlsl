#include "CSUtils.hlsli"

#define L 10
#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint pCount;
    float globallCellWidth;
    float sPadding[2];
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
RWStructuredBuffer<uint> liBuffer : register(u2);
RWStructuredBuffer<uint> lcBuffer : register(u3);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    const uint ni = counterBuffer[1];
    const uint nl = counterBuffer[0];
    const uint n = ni + nl;
    
    if (index >= n)
        return;
    
    uint ws = nkBuffer[index];
    uint level = GetTreeLevelFromWs(ws);
    uint prefix = ws - (1 << (3 * level));
    uint range = 3 * (L - level);
    
    uint begin = prefix << range;
    uint end = (prefix + 1) << range;
    
    uint indexBegin = BinarySearch(begin, keyBuffer, pCount);
    uint indexLast = BinarySearch(end, keyBuffer, pCount);
    uint count = indexLast - indexBegin;
    
    float mass = 0.f;
    float3 center = float3(0.f, 0.f, 0.f);
        
    for (uint i = indexBegin; i < indexLast; ++i)
    {
        const float iMass = massBuffer[i];
        mass += iMass;
        
        const float3 iPos = posBuffer[i];
        center.x += iPos.x * iMass;
        center.y += iPos.y * iMass;
        center.z += iPos.z * iMass;
    }
    
    if (mass > 0.f)
        centerOfMass[index] = center / mass;
            
    combinedMass[index] = mass;
            
    liBuffer[index] = indexBegin;
    lcBuffer[index] = count;
}