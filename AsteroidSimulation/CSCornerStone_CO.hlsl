#define L 10
#define NUM_THREADS 256

#include "CSUtils.hlsli"

StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> nkBuffer : register(t1);
StructuredBuffer<uint> loBuffer : register(t2);

RWStructuredBuffer<uint> coBuffer : register(u0);

uint GetChildIndex(const uint nkVal, const uint n)
{
    const uint firstChildValue = nkVal << 3;
    
    const uint leafLevel = GetTreeLevelFromWs(nkVal);
    const uint leafLevelSize = 1 << 3 * leafLevel;
    const uint leafLevelChild = min(leafLevel + 1, L + 1);
    const uint nextLeafLevelChild = min(leafLevelChild + 1, L + 1);
    
    const uint childLeafLevelOffset = loBuffer[leafLevelChild];
    const uint childIndexLimit = loBuffer[nextLeafLevelChild];
    
    uint firstChildIndex = BinarySearchRange(firstChildValue, nkBuffer, childLeafLevelOffset, childIndexLimit);
    
    if (nkBuffer[firstChildIndex] != firstChildValue || firstChildIndex >= n)
        firstChildIndex = 0;
    
    return firstChildIndex;
}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    
    uint ni = counterBuffer[1];
    uint nl = counterBuffer[0];
    uint n = counterBuffer[0];
    
    if (index >= n)
        return;
    
    const uint nkVal = nkBuffer[index];
    
    uint firstChildIndex = GetChildIndex (nkVal, n);
    
    coBuffer[index] = firstChildIndex;
}