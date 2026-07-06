#include "CSUtils.hlsli"

#define NUM_THREADS 256

StructuredBuffer<uint> inOBuffer : register(t0);
StructuredBuffer<uint> counterBuffer : register(t1);
RWStructuredBuffer<uint> outOBuffer : register(u0);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    uint nl = counterBuffer[0];
    
    if (index > nl)
        return;
    
    uint localCounter = 0;
    
    for (uint i = 0; i < index; ++i)
        localCounter += inOBuffer[i];
    
    outOBuffer[index] = localCounter;
}