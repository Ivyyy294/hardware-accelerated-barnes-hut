#include "CSUtils.hlsli"
#include "CSZOrder.hlsli"

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
RWBuffer<float4> cellMpData : register(u0);
RWStructuredBuffer<uint> liBuffer : register(u1);
RWStructuredBuffer<uint> lcBuffer : register(u2);
RWBuffer<float4> cellGeoData : register(u3);

groupshared bool gAbort;

groupshared uint gIndexBegin;
groupshared uint gIndexEnd;
groupshared uint gCount;
groupshared float4 gLocalComMass[NUM_THREADS];

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    if (GTid.x == 0)
    {
        const uint ni = counterBuffer[1];
        const uint nl = counterBuffer[0];
        const uint n = ni + nl;
        gAbort = Gid.x >= n;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    if (gAbort)
        return;
    
    const uint index = Gid.x;

    if (GTid.x == 0)
    {
        uint ws = nkBuffer[index];
        uint level = GetTreeLevelFromWs(ws);
        uint prefix = ws - (1 << (3 * level));
        uint range = 3 * (L - level);
    
        uint begin = prefix << range;
        uint end = (prefix + 1) << range;
        
        float cellWidth = globallCellWidth / (1 << level);
    
        float3 boundsMin = globallCellWidth * -0.5f;
        float3 boundsMax = globallCellWidth * 0.5f;
    
        cellGeoData[index] = float4(DecodeWSKey(ws, level, boundsMin, boundsMax), cellWidth);
        
        gIndexBegin = BinarySearch(begin, keyBuffer, pCount);
        gIndexEnd = BinarySearch(end, keyBuffer, pCount);
        gCount = gIndexEnd - gIndexBegin;
        
        liBuffer[index] = gIndexBegin;
        lcBuffer[index] = gCount;
    }
    
    GroupMemoryBarrierWithGroupSync();
        
    float4 localComMass = 0.f;
    
    for (uint i = gIndexBegin + GTid.x; i < gIndexEnd; i += NUM_THREADS)
    {
        const float iMass = massBuffer[i];
        localComMass.w += iMass;
        
        const float3 iPos = posBuffer[i];
        localComMass.x += iPos.x * iMass;
        localComMass.y += iPos.y * iMass;
        localComMass.z += iPos.z * iMass;
    }
    
    gLocalComMass[GTid.x] = localComMass;
    
    GroupMemoryBarrierWithGroupSync();
    
    if (GTid.x == 0)
    {
        float3 com = 0.f;
        float mass = 0.f;
        
        for (uint i = 0; i < NUM_THREADS; ++i)
        {
            com += gLocalComMass[i].xyz;
            mass += gLocalComMass[i].w;
        }
        
        if (mass > 0.f)
            com = com / mass;
        
        cellMpData[index] = float4(com, mass);
    }
}