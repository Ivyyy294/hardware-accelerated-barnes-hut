#include "CSUtils.hlsli"
#define NUM_THREADS 256

StructuredBuffer<uint> inOBuffer : register(t0);
StructuredBuffer<uint> counterBuffer : register(t1);
RWStructuredBuffer<uint> outOBuffer : register(u0);

groupshared uint localCounter[NUM_THREADS];
groupshared uint localStatus[NUM_THREADS];
groupshared uint groupStartCount = 0;

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    uint kSize = counterBuffer[0];
    uint gCount = (kSize + NUM_THREADS - 1) / NUM_THREADS;
    
    localCounter[GTid.x] = index > 0 && index < kSize ? inOBuffer[index - 1] : 0;
    localStatus[GTid.x] = GTid.x == 0;
    
    GroupMemoryBarrierWithGroupSync();

    //Chained local Add
    for (uint offset = 1; offset < NUM_THREADS; offset <<= 1)
    {
        uint add = 0;
        
        if (GTid.x > offset)
            add = localCounter[GTid.x - offset];
        
        localCounter[GTid.x] += add;
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    //Copy local group size into global memmory
    if (GTid.x == 0)
        outOBuffer[Gid.x] = localCounter[NUM_THREADS - 1];
    
    GroupMemoryBarrierWithGroupSync();

    //Propage group offset
    for (uint gOffset = 1; gOffset < gCount; gOffset <<= 1)
    {
        uint add = 0;
        
        if (Gid.x >= gOffset && GTid.x == 0)
        {
            add = outOBuffer[Gid.x - gOffset];
            //ebugBuffer[index] = add;
            groupStartCount = groupStartCount + add;
        }
        GroupMemoryBarrierWithGroupSync();
    }
    
    //Global Add
    if (index < kSize)
        outOBuffer[index] = groupStartCount + localCounter[GTid.x];
}