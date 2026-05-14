#include "CSUtils.hlsli"

StructuredBuffer<uint> inKBuffer : register(t0);
StructuredBuffer<uint> inOBuffer : register(t1);
StructuredBuffer<uint> inO2Buffer : register(t2);
RWStructuredBuffer<uint> outKBuffer : register(u0);
RWStructuredBuffer<uint> counterBuffer : register(u1);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    uint kSize = counterBuffer[0];
    
    if (index == 0)
    {
        counterBuffer[0] = 0;
        counterBuffer[2] = 0;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    if (index <= kSize)
    {
        uint kVal = inKBuffer[index];
        uint oVal = inOBuffer[index];

        uint outKIndex = inO2Buffer[index];

        if (oVal == 1)
            outKBuffer[outKIndex] = kVal;
        else if (oVal == 8)
        {
            uint kDelta = inKBuffer[index + 1] - kVal;
            uint iDelta = kDelta / 8;
        
            for (int j = 0; j < 8; ++j)
                outKBuffer[outKIndex + j] = kVal + j * iDelta;
        }
        
        if (oVal != 1)
            InterlockedAdd(counterBuffer[2], 1);
    
        GroupMemoryBarrierWithGroupSync();
    
        if (index < kSize)
            InterlockedAdd(counterBuffer[0], oVal);
    }
    
    GroupMemoryBarrierWithGroupSync();
    //debugBuffer[index] = kSize;
    //outKBuffer[0] = counterBuffer[0];
}