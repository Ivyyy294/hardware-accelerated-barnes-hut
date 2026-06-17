
RWStructuredBuffer<uint> debugBuffer : register(u7);

uint BinarySearch(uint key, StructuredBuffer<uint> buffer, uint bufferSize)
{
    uint low = 0;
    uint high = bufferSize;
    
    while (low < high)
    {
        //easy uint division by 2
        uint mid = (low + high) >> 1;
        uint mid_val = buffer[mid];
        
        if (mid_val < key)
            low = mid + 1;
        else
            high = mid;
    }
    
    return low;
}

uint BinarySearchRange(uint key, StructuredBuffer<uint> buffer, uint start, uint end)
{
    uint low = start;
    uint high = end;
    
    while (low < high)
    {
        //easy uint division by 2
        uint mid = (low + high) >> 1;
        uint mid_val = buffer[mid];
        
        if (mid_val < key)
            low = mid + 1;
        else
            high = mid;
    }
    
    return low;
}

uint Clz30 (uint val)
{
    return 29 - firstbithigh(val);
}

uint GetTreeLevelFromWs(uint ws)
{
    const uint msb = firstbithigh(ws);
    return msb / 3;
}


uint FloatToUint(const float val)
{
    uint uvalue = asuint(val);
    uvalue = (uvalue & 0x80000000u) ? ~uvalue : (uvalue ^ 0x80000000u);
    return uvalue;
}

float UintToFloat(uint val)
{
    val = (val & 0x80000000u) ? (val & ~0x80000000u) : ~val;
    return asfloat(val);
}
