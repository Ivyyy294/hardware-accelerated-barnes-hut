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

groupshared float gCombinedMass[NUM_THREADS];
groupshared float3 gCenterOfMass[NUM_THREADS];
groupshared uint gCount = 0;
groupshared uint gIndexBegin = 0;
groupshared bool gIsLeaf = false;

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    if (GTid.x == 0)
    {
        uint ws = nkBuffer[Gid.x];
        uint level = GetTreeLevelFromWs(ws);
        uint prefix = ws - (1 << (3 * level));
        uint range = 3 * (L - level);
    
        uint begin = prefix << range;
        uint end = (prefix + 1) << range;
    
        gIndexBegin = BinarySearch(begin, keyBuffer, pCount);
        uint indexLast = BinarySearch(end, keyBuffer, pCount);
        gCount = indexLast - gIndexBegin;
        
        liBuffer[Gid.x] = gIndexBegin;
        lcBuffer[Gid.x] = gCount;
        gIsLeaf = coBuffer[Gid.x] == 0;
    }
    
    AllMemoryBarrierWithGroupSync();
    
    if (gIsLeaf)
    {
        if (GTid.x < gCount)
        {
            const uint pIndex = gIndexBegin + GTid.x;
            const float mass = massBuffer[pIndex];
            float3 pos = posBuffer[pIndex];
        
            gCombinedMass[GTid.x] = mass;
            gCenterOfMass[GTid.x] = pos * mass;
        }
        
        AllMemoryBarrierWithGroupSync();
        
        if (GTid.x == 0)
        {
            float mass;
            float3 pos;
            
            for (uint i = 0; i < gCount; ++i)
            {
                mass += gCombinedMass[i];
                pos += gCenterOfMass[i];
            }
            
            centerOfMass[Gid.x] = mass > 0.f ? pos / mass : float3(0.f, 0.f, 0.f);
            combinedMass[Gid.x] = mass;
        }
    }
    
    //for (uint i = 0; i < count; i += NUM_THREADS)
    //{
    //    const uint pOffset = GTid.x + i;
    //    const uint pIndex = indexBegin + pOffset;
        
    //    if (pOffset < gCount)
    //    {
    //        float mass = massBuffer[pIndex];
    //        InterlockedAdd(gCombinedMass, asuint(mass));
            
    //        float3 pos = posBuffer[pIndex];
    //        InterlockedAdd(gCenterOfMass[0], asuint(pos.x * mass));
    //        InterlockedAdd(gCenterOfMass[1], asuint(pos.y * mass));
    //        InterlockedAdd(gCenterOfMass[2], asuint(pos.z * mass));
    //    }
    //}
    
    //GroupMemoryBarrierWithGroupSync();
    
    //if (GTid.x == 0)
    //{
    //    const float mass = asfloat(gCombinedMass);
    //    combinedMass[Gid.x] = mass;
        
    //    if (mass > 0.f)
    //    {
    //        float3 center = 0.f;
    //        center.x = asfloat(gCenterOfMass[0]);
    //        center.y = asfloat(gCenterOfMass[1]);
    //        center.z = asfloat(gCenterOfMass[2]);
    //        centerOfMass[Gid.x] = center / mass;
    //    }
    //    else
    //        centerOfMass[Gid.x] = float3(0.f, 0.f, 0.f);
    //}
}