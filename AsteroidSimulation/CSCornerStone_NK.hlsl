
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
    uint index = DTid.x;

    if (index >= kSize)
        return;
    
    uint k = kBuffer[index];
    uint k1 = kBuffer[index + 1];
    uint keyRange = k1 - k;
    uint L = 10;
    //A key at level l covers 8^L-l keys
    //L = 10
    
    uint octalBitsCount = floor(firstbithigh(keyRange) / 3);
    uint leafLevel = 10 - octalBitsCount;
    uint octalBits = k >> octalBitsCount * 3;
    uint wsKey = (1 << (leafLevel * 3)) | octalBits;
    debugBuffer[index] = kSize;
    
    //CountLeadingZeros()
    //uint test = k ^ k1;
    
}