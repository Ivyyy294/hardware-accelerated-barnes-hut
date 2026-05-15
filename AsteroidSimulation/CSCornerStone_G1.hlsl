#include "CSUtils.hlsli"

#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint nCrit;
    uint sPadding[3];
}

StructuredBuffer<uint> kBuffer : register(t0);
StructuredBuffer<uint> nBuffer : register(t1);
StructuredBuffer<uint> counterBuffer : register(t2);
RWStructuredBuffer<uint> oBuffer : register(u0);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    //Thread index
    uint index = Gid.x * NUM_THREADS + GTid.x;
    
    uint kSize = counterBuffer[0];
 
    //Early exit
    if (index > kSize)
        return;

    uint nVal = nBuffer[index];
    uint kVal = kBuffer[index];
    uint k1Val = kBuffer[index + 1];
    uint kDelta = k1Val - kVal;
    uint parentDelta = (8 * kDelta);
    
    uint indexJ = (kVal % parentDelta) / kDelta;
    uint indexFirstSibling = index - indexJ;
    
    uint combinedParticleCount = 0;
    
    for (uint i = indexFirstSibling; i < indexFirstSibling + 8; ++i)
        combinedParticleCount += nBuffer[i];
    
    bool split = nVal > nCrit;
    bool firstOctant = kVal % parentDelta == 0;
    bool consecutive = kBuffer[indexFirstSibling] + parentDelta == kBuffer[indexFirstSibling + 8];
    bool combine = !firstOctant && consecutive && combinedParticleCount < nCrit;

    if (split)
        oBuffer[index] = 8;
    else if (combine)
        oBuffer[index] = 0;
    else
        oBuffer[index] = 1;
}