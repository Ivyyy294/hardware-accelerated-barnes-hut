#include "CSUtils.hlsli"

#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint pSize;
    uint sPadding[3];
}

StructuredBuffer<uint> pBuffer : register(t0);
StructuredBuffer<uint> kBuffer : register(t1);
StructuredBuffer<uint> counterBuffer : register(t2);
RWStructuredBuffer<uint> nBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint intervalIndex = Gid.x * NUM_THREADS + GTid.x;
    
    uint nl = counterBuffer[0];
 
    //debugBuffer[intervalIndex] = counterBuffer[2];
    //debugBuffer[intervalIndex] = pBuffer[intervalIndex];
    //debugBuffer[intervalIndex] = kBuffer[intervalIndex];
    
    //Kuffer has a length of nl + 1
    if (intervalIndex > nl)
        return;
 
    uint k0 = kBuffer[intervalIndex];
    uint k1 = kBuffer[intervalIndex + 1];
    
    uint low = BinarySearch(k0, pBuffer, pSize);
    uint high = BinarySearch(k1, pBuffer, pSize);
    
    nBuffer[intervalIndex] = high >= low ? (high - low) : 0;
    //debugBuffer[intervalIndex] = nBuffer[intervalIndex];
}