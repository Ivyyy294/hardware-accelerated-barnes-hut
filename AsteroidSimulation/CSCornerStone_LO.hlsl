#include "CSUtils.hlsli"

#define L 10
#define NUM_THREADS 256

StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> nkBuffer : register(t1);

RWStructuredBuffer<uint> loBuffer : register(u0);

//ToDo: fix indexFirstNodeOnLevel for index > 10
[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    
    if (index > L + 2)
        return;
    
    uint ni = counterBuffer[1];
    uint nl = counterBuffer[0];
    uint n = ni + nl;
    
    uint bufferSize = counterBuffer[0];
    uint searchVal = 1 << 3 * index;
    
    uint indexFirstNodeOnLevel = BinarySearch(searchVal, nkBuffer, n);
    loBuffer[index] = indexFirstNodeOnLevel;
    
    if (index > L)
        loBuffer[index] = n;
}