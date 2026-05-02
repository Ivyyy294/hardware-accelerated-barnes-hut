#define NUM_BINS 256
#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint elementCount;
    uint dispatchGroupCount;
    uint digitPlace;
    uint sPadding[1];
}

StructuredBuffer<uint> exclusivePrefixSum : register(t0);
StructuredBuffer<uint> mortonCodeIn : register(t1);

RWStructuredBuffer<uint> scatterCounters : register(u0);
RWStructuredBuffer<uint> mortonCodeOut : register(u1);
RWStructuredBuffer<uint> debugBuffer : register(u2);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    //debugBuffer[index] = exclusivePrefixSum[index];
    //debugBuffer[index] = mortonCodeIn[index];
    if (index >= elementCount)
        return;
    
    //Get value
    uint key = mortonCodeIn[index];
    
    //Get radix
    uint shift = digitPlace * 8;
    uint digit = (key >> shift) & 0xFF;
    
    //Count prior matching digits
    uint rank = 0;
    
    for (uint i = 0; i < index; ++i)
    {
        uint other = mortonCodeIn[i];
        uint otherDigit = (other >> shift) & 0xFF;

        if (otherDigit == digit)
            rank++;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    //Get histogram index
    uint histIndex = digitPlace * NUM_BINS + digit;
    
    //Final destination
    uint dstIndex = exclusivePrefixSum[histIndex] + rank;

    //Scatter
    if (dstIndex < elementCount)
    {
        mortonCodeOut[dstIndex] = key;
    }
    
    GroupMemoryBarrierWithGroupSync();
    //debugBuffer[index] = mortonCodeOut[index];
    //debugBuffer[index] = elementCount;
}