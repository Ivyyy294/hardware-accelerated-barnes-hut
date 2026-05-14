#define L 10
#define NUM_THREADS 256

#include "CSUtils.hlsli"

StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> nkBuffer : register(t1);
StructuredBuffer<uint> loBuffer : register(t2);

RWStructuredBuffer<uint> coBuffer : register(u0);

uint GetChildIndex1(const uint nkVal, const uint n)
{
    const uint leafLevel = GetTreeLevelFromWs(nkVal);
    const uint leafLevelSize = 1 << 3 * leafLevel;
    const uint leafLevelChild = min(leafLevel + 1, L);
    const uint nextLeafLevelChild = min(leafLevelChild + 1, L);
    
    const uint childLeafLevelOffset = loBuffer[leafLevelChild];
    const uint childIndexLimit = loBuffer[nextLeafLevelChild];
    
    const uint nodeIndexOffset = (nkVal - leafLevelSize) * 8;

    uint firstChildIndex = childLeafLevelOffset + nodeIndexOffset;
    
    if (firstChildIndex >= childIndexLimit)
        firstChildIndex = 0;
    
    return firstChildIndex;
}

uint GetChildIndex2(const uint nkVal, const uint n)
{
    const uint firstChildValue = nkVal << 3;
    
    const uint leafLevel = GetTreeLevelFromWs(nkVal);
    const uint leafLevelSize = 1 << 3 * leafLevel;
    const uint leafLevelChild = min(leafLevel + 1, L);
    const uint nextLeafLevelChild = min(leafLevelChild + 1, L);
    
    const uint childLeafLevelOffset = loBuffer[leafLevelChild];
    const uint childIndexLimit = loBuffer[nextLeafLevelChild];
    
    uint firstChildIndex = BinarySearchRange(firstChildValue, nkBuffer, childLeafLevelOffset, childIndexLimit);
    
    if (nkBuffer[firstChildIndex] != firstChildValue)
        firstChildIndex = 0;
    
    return firstChildIndex;
}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    //debugBuffer[index] = nkBuffer[index];
    uint ni = counterBuffer[1];
    uint nl = counterBuffer[0];
    uint n = counterBuffer[0];
    
    //debugBuffer[index] = loBuffer[index];
    //debugBuffer[index] = nkBuffer[index];
    
    if (index >= n)
    {
        //debugBuffer[index] = 0;
        return;
    }
    //debugBuffer[index] = nkBuffer[index];
    
    const uint nkVal = nkBuffer[index];
    
    //uint firstChildIndex = GetChildIndex1(nkVal);
    uint firstChildIndex = GetChildIndex2(nkVal, n);
    
    //debugBuffer[index] = GetChildIndex2(nkVal, n);
    
    coBuffer[index] = firstChildIndex;
    //debugBuffer[index] = nodeIndexOffset;
    //debugBuffer[index] = childLeafLevelOffset;
    //debugBuffer[index] = leafLevel;
    //debugBuffer[index] = coBuffer[index];
    //debugBuffer[index] = ni;
    //debugBuffer[index] = leafLevelSize;
}