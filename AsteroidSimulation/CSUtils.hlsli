
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