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

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    //Particle index
    uint index = Gid.x * NUM_THREADS + GTid.x;
    
    //Number of leaf nodes
    uint nl = counterBuffer[0];
    
    //K Buffer has a length of nl + 1
    //Early exit if particle index is out of bounds
    if (index >= nl)
        return;
 
    uint k0 = kBuffer[index];
    uint k1 = kBuffer[index + 1];
    
    uint low = BinarySearch(k0, pBuffer, pSize);
    uint high = BinarySearch(k1, pBuffer, pSize);
    
    nBuffer[index] = high >= low ? (high - low) : 0;
}