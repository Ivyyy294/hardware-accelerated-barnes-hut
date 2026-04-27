cbuffer SettingsBuffer : register(b0)
{
    uint kSize;
    uint sPadding[3];
}

StructuredBuffer<uint> inKBuffer : register(t0);
StructuredBuffer<uint> inOBuffer : register(t1);
StructuredBuffer<uint> inO2Buffer : register(t2);
RWStructuredBuffer<uint> outKBuffer : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    if (index > kSize - 1)
        return;
    
    uint kVal = inKBuffer[index];
    uint oVal = inOBuffer[index];
    
    if (oVal == 1)
    {
        uint outKIndex = inO2Buffer[index];
        outKBuffer[outKIndex] = kVal;
    }
    else if (oVal == 8)
    {
        uint kDelta = inKBuffer[index + 1] - kVal;
        uint iDelta = kDelta / 8;
        
        for (int j = 0; j < 8; ++j)
            outKBuffer[index + j] = kVal + j * iDelta;
    }
}