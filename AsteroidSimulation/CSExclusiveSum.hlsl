#define NUM_BINS 256
#define NUM_DIGITS 4

RWStructuredBuffer<uint> globalHistogram : register(u0);
RWStructuredBuffer<uint> exclusivePrefixSum : register(u1);

[numthreads(256, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint startIndex = Gid.x * NUM_BINS;
    const uint targetIndex = startIndex + GTid.x;

    if (targetIndex < NUM_BINS * NUM_DIGITS)
    {
        uint counter = 0;
    
        for (uint i = startIndex; i < targetIndex; i++)
            counter += globalHistogram[i];
        
        InterlockedAdd(exclusivePrefixSum[targetIndex], counter);
    }
    
    GroupMemoryBarrierWithGroupSync();
}