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

uint BinarySearch(uint key)
{
    uint low = 0;
    uint high = pSize;
    
    while (low < high)
    {
        //easy uint division by 2
        uint mid = (low + high) >> 1;
        uint mid_val = pBuffer[mid];
        
        if (mid_val < key)
            low = mid + 1;
        else
            high = mid;
    }
    
    return low;
}

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint intervalIndex = DTid.x;
    
    uint kSize = counterBuffer[0];
 
    if (intervalIndex >= kSize)
        return;
 
    uint k0 = kBuffer[intervalIndex];
    uint k1 = kBuffer[intervalIndex + 1];
    
    uint low = BinarySearch(k0);
    uint high = BinarySearch(k1);
    
    nBuffer[intervalIndex] = high >= low ? (high - low) : 0;
}