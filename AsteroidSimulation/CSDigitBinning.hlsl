#define ITEM_PER_THREAD 8
#define NUM_THREADS 256
#define WARP_SIZE 32
#define NUM_WAPRS (NUM_THREADS / WARP_SIZE)

cbuffer Settings : register(b0)
{
    uint elementCount;
    uint digitPlace;
    uint sPadding[2];
}

RWStructuredBuffer<uint> tileCounter : register(u0);

//could be read only
RWStructuredBuffer<uint> exclusivePrefixSum : register(u1);

RWStructuredBuffer<uint> mortonCodes : register(u2);
RWStructuredBuffer<float3> position : register(u3);
RWStructuredBuffer<float> mass : register(u4);

groupshared uint tileId;
groupshared uint g_digits[NUM_THREADS];

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
        uint3 Gid : SV_GroupID)
{
    const uint tileSize = ITEM_PER_THREAD * NUM_THREADS;
    
    //Thread 0 grabs a tile for the thread block
    
    if (GTid.x == 0)
        InterlockedAdd(tileCounter[0], 1, tileId);
    
    GroupMemoryBarrierWithGroupSync();
    
    const uint localTileId = tileId;
    const uint baseIndex = localTileId * tileSize;

    //thread index within a lane
    const uint laneId = GTid.x % 32;
    
    //index of the warp
    const uint warpId = GTid.x / 32;
    uint warpBase = warpId * 32;
    
    //Get digits for all items
    for (uint item = 0; item < ITEM_PER_THREAD; ++item)
    {
        uint index = baseIndex + item * NUM_THREADS + GTid.x;
        uint myDigit = 0;
        
        if (index < elementCount)
        {
            uint key = mortonCodes[index];
            myDigit = (key >> (digitPlace * 8)) & 0xFF;
        }
        
        g_digits[GTid.x] = myDigit;

        GroupMemoryBarrierWithGroupSync();

        uint mask = 0;

        for (uint lane = 0; lane < 32; lane++)
        {
            if (g_digits[warpBase + lane] == myDigit)
                mask |= (1u << lane);
        }
        
        uint rank = countbits(mask & ((1u << laneId) - 1));
        uint groupSize = countbits(mask);
    }
}