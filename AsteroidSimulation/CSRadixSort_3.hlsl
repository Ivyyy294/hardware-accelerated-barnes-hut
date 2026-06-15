#include "CSUtils.hlsli"

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
StructuredBuffer<float3> positionIn : register(t2);
StructuredBuffer<float> massIn : register(t3);
StructuredBuffer<uint> newToOldIndexIn : register(t4);

RWStructuredBuffer<uint> mortonCodeOut : register(u0);
RWStructuredBuffer<float3> positionOut : register(u1);
RWStructuredBuffer<float> massOut : register(u2);
RWStructuredBuffer<uint> newToOldIndexOut : register(u3);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;

    if (index >= elementCount)
        return;
    
    //Get value
    uint key = mortonCodeIn[index];
    float3 position = positionIn[index];
    float mass = massIn[index];
    uint oldIndex = digitPlace == 0 ? index : newToOldIndexIn[index];
    
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
        positionOut[dstIndex] = position;
        massOut[dstIndex] = mass;
        newToOldIndexOut[dstIndex] = oldIndex;
    }
    
    //debugBuffer[index] = newToOldIndexOut[index];
}