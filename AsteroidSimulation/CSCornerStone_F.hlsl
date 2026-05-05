#include "CSUtils.hlsli"

cbuffer SettingsBuffer : register(b0)
{
    uint pSize;
    uint sPadding[3];
}

StructuredBuffer<uint> pBuffer : register(t0);
StructuredBuffer<uint> kBuffer : register(t1);
StructuredBuffer<uint> counterBuffer : register(t2);
RWStructuredBuffer<uint> nBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint intervalIndex = DTid.x;
    
    uint kSize = counterBuffer[0];
 
    //debugBuffer[intervalIndex] = pBuffer[intervalIndex];
    //debugBuffer[intervalIndex] = kBuffer[intervalIndex];
    
    if (intervalIndex >= kSize)
        return;
 
    uint k0 = kBuffer[intervalIndex];
    uint k1 = kBuffer[intervalIndex + 1];
    
    uint low = BinarySearch(k0, pBuffer, pSize);
    uint high = BinarySearch(k1, pBuffer, pSize);
    
    nBuffer[intervalIndex] = high >= low ? (high - low) : 0;
    //debugBuffer[intervalIndex] = nBuffer[intervalIndex];
}