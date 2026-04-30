#define L 10
#define NUM_THREADS 256

StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> nkBuffer : register(t1);

RWStructuredBuffer<uint> loBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

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

//ToDo: fix indexFirstNodeOnLevel for index > 10
[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint index = Gid.x * NUM_THREADS + GTid.x;
    
    //debugBuffer[index] = nkBuffer[index];
    
    if (index >= L + 2)
        return;
    
    uint bufferSize = 25; //counterBuffer[0];
    uint searchVal = 1 << 3 * index;
    //debugBuffer[index] = searchVal;
    
    uint indexFirstNodeOnLevel = BinarySearch(searchVal, nkBuffer, bufferSize);
    loBuffer[index] = indexFirstNodeOnLevel;
    
    GroupMemoryBarrierWithGroupSync();
    //debugBuffer[index] = loBuffer[index];
}