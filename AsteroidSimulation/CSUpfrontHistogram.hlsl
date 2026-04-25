#define NUM_BINS 256
#define NUM_DIGITS 4

#define ITEM_PER_THREAD 8
#define NUM_THREADS 256

//Globals
cbuffer SettingsBuffer : register(b0)
{
    uint elementCount;
    uint dispatchGroupCount;
    uint sPadding[2];
}

StructuredBuffer<uint> mortonCodes : register(t0);
RWStructuredBuffer<uint> globalHistogram : register(u0);

groupshared uint localHistogram[NUM_DIGITS * NUM_BINS];

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID,
          uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
   
    const uint tileSize = ITEM_PER_THREAD * NUM_THREADS;
    uint histSize = NUM_DIGITS * NUM_BINS;
    const uint groupId = Gid.x;
    const uint threadId = GTid.x;
    
    //Clean local histogram
    for (uint i = threadId; i < histSize; i += NUM_THREADS)
        localHistogram[i] = 0;
    
    //Blocks execution of all threads in a group until all group shared accesses have been completed
    //and all threads in the group have reached this call.
    GroupMemoryBarrierWithGroupSync();
    
    for (uint tileIndex = groupId; tileIndex * tileSize < elementCount; tileIndex += dispatchGroupCount)
    {
        const uint baseIndex = tileIndex * tileSize;

        for (uint i = 0; i < ITEM_PER_THREAD; ++i)
        {
            uint index = baseIndex + threadId + i * NUM_THREADS;
            
            if (index < elementCount)
            {
                uint val = mortonCodes[index];
                
                //Get digit per position
                uint digit0 = (val >> 0) & 0xFF;
                uint digit1 = (val >> 8) & 0xFF;
                uint digit2 = (val >> 16) & 0xFF;
                uint digit3 = (val >> 24) & 0xFF;
                
                //Add digits to local histogram
                InterlockedAdd(localHistogram[0 * NUM_BINS + digit0], 1);
                InterlockedAdd(localHistogram[1 * NUM_BINS + digit1], 1);
                InterlockedAdd(localHistogram[2 * NUM_BINS + digit2], 1);
                InterlockedAdd(localHistogram[3 * NUM_BINS + digit3], 1);
            }
        }
    }

    //Sync point
    GroupMemoryBarrierWithGroupSync ();
    
    //Add local histogram values to global histogram
    for (uint i = threadId; i < histSize; i += NUM_THREADS)
    {
        uint value = localHistogram[i];
        
        if (value > 0)
            InterlockedAdd(globalHistogram[i], value);
    }
    
    //Sync point
    GroupMemoryBarrierWithGroupSync();
}