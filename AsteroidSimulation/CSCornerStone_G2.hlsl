cbuffer SettingsBuffer : register(b0)
{
    uint kSize;
    uint sPadding[3];
}

StructuredBuffer<uint> inOBuffer : register(t0);
RWStructuredBuffer<uint> outOBuffer : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    if (index > kSize)
        return;
    
    int localCounter = 0;
    
    for (uint i = 0; i < index; ++i)
        localCounter += inOBuffer[i];
    
    outOBuffer[index] = localCounter;
}