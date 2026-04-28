StructuredBuffer<uint> inKBuffer : register(t0);
StructuredBuffer<uint> inOBuffer : register(t1);
StructuredBuffer<uint> inO2Buffer : register(t2);
RWStructuredBuffer<uint> outKBuffer : register(u0);
RWStructuredBuffer<uint> counterBuffer : register(u1);
RWStructuredBuffer<uint> debugBuffer : register(u2);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    uint kSize = counterBuffer[0];
    
    if (index == 0)
        counterBuffer[0] = 0;
    
    GroupMemoryBarrierWithGroupSync();
    
    
    if (index > kSize)
        return;
    
    uint kVal = inKBuffer[index];
    uint oVal = inOBuffer[index];

    uint outKIndex = inO2Buffer[index];

    if (oVal == 1)
    {
        outKBuffer[outKIndex] = kVal;
        debugBuffer[outKIndex] = kVal;
        InterlockedAdd(counterBuffer[0], 1);
    }
    else if (oVal == 8)
    {
        uint kDelta = inKBuffer[index + 1] - kVal;
        uint iDelta = kDelta / 8;
        
        for (int j = 0; j < 8; ++j)
        {
            outKBuffer[outKIndex + j] = kVal + j * iDelta;
            debugBuffer[outKIndex + j] = kVal + j * iDelta;
            InterlockedAdd(counterBuffer[0], 1);
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    //outKBuffer[0] = counterBuffer[0];
}