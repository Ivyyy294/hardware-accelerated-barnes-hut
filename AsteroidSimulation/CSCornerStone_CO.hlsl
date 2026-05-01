#define L 10
#define NUM_THREADS 256

StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> nkBuffer : register(t1);
StructuredBuffer<uint> loBuffer : register(t2);

RWStructuredBuffer<uint> coBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    //debugBuffer[index] = nkBuffer[index];
    uint ni = counterBuffer[1] - 1;
    
    if (index >= ni)
    {
        //debugBuffer[index] = 0;
        return;
    }
    //debugBuffer[index] = nkBuffer[index];
    
    const uint nkVal = nkBuffer[index];
    const uint leafLevel = floor(firstbithigh(nkVal) / 3);
    const uint leafLevelSize = 1 << 3 * leafLevel;
    const uint leafLevelChild = leafLevel + 1;
    
    const uint childLeafLevelOffset = loBuffer[leafLevelChild];
    
    const uint nodeIndexOffset = (nkVal - leafLevelSize) * leafLevelSize;
    
    const uint firstChildIndex = childLeafLevelOffset + nodeIndexOffset;
    
    coBuffer[index] = firstChildIndex;
    //debugBuffer[index] = counterBuffer[0];
}