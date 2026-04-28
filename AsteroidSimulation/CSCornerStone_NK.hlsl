
StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> kBuffer : register(t1);
RWStructuredBuffer<uint> nkBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

uint Log8(uint val)
{
    uint msb = 31 - firstbithigh(val);
    return msb / 3;
}

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint kSize = counterBuffer[0];
    uint leafCount = kSize - 1;
    uint index = DTid.x;

    if (index > leafCount)
        return;
    
    uint k = kBuffer[index];
    uint k1 = kBuffer[index + 1];
    uint kDelta = k1 - k;
    uint L = 10;
    uint msd = 10 - floor(firstbithigh(kDelta) / 3);
    
    debugBuffer[index] = firstbithigh(4);
    //CountLeadingZeros()
    //uint test = k ^ k1;
    
}