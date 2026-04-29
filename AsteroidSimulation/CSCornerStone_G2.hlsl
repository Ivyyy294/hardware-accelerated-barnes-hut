StructuredBuffer<uint> inOBuffer : register(t0);
StructuredBuffer<uint> counterBuffer : register(t1);
RWStructuredBuffer<uint> outOBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    uint kSize = counterBuffer[0];
    
    if (index > kSize)
        return;
    
    uint localCounter = 0;
    
    for (uint i = 0; i < index; ++i)
        localCounter += inOBuffer[i];
    
    outOBuffer[index] = localCounter;
}