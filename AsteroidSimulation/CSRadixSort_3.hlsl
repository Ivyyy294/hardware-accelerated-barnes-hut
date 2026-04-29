//#define ITEM_PER_THREAD 8
//#define NUM_THREADS 256
//#define WARP_SIZE 32
//#define NUM_WARPS (NUM_THREADS / WARP_SIZE)

//cbuffer SettingsBuffer : register(b0)
//{
//    uint elementCount;
//    uint dispatchGroupCount;
//    uint digitPlace;
//    uint sPadding[1];
//}

//cbuffer Settings : register(b0)
//{
//    uint elementCount;
//    uint digitPlace;
//    uint sPadding[2];
//}

//RWStructuredBuffer<uint> tileCounter : register(u0);

////could be read only
//RWStructuredBuffer<uint> exclusivePrefixSum : register(u1);
//RWStructuredBuffer<uint> scanState : register(u2);

//RWStructuredBuffer<uint> mortonCodes : register(u3);
//RWStructuredBuffer<float3> position : register(u4);
//RWStructuredBuffer<float> mass : register(u5);


//groupshared uint tileId;

////Per item data
//groupshared uint g_digits[NUM_THREADS];
//groupshared uint g_ranks[NUM_THREADS];

//// Histograms
//groupshared uint warpHist[NUM_WARPS][NUM_THREADS];
//groupshared uint blockHist[NUM_THREADS];
//groupshared uint blockPrefix[NUM_THREADS];
//groupshared uint blockScanPrefix[NUM_THREADS];

//void BuildWarpHistogram (uint tileId, uint3 GTid)
//{
//    const uint tileSize = ITEM_PER_THREAD * NUM_THREADS;
//    const uint baseIndex = tileId * tileSize;
    
//    const uint laneId = GTid.x % WARP_SIZE;
//    const uint warpId = GTid.x / WARP_SIZE;
//    const uint warpBase = warpId * WARP_SIZE;
    
//    for (uint item = 0; item < ITEM_PER_THREAD; item++)
//    {
//        uint index = baseIndex + item * NUM_THREADS + GTid.x;
//        uint digit = 0;
        
//        //Retrieve digit
//        if (index < elementCount)
//        {
//            uint key = mortonCodes[index];
//            digit = (key >> (digitPlace * 8)) & 0xFF;
//        }
        
//        g_digits[GTid.x] = digit;
        
//        GroupMemoryBarrierWithGroupSync();
        
//        uint mask = 0;
        
//        //Build digit mask
//        for (uint lane = 0; lane < WARP_SIZE; lane++)
//        {
//            if (g_digits[item * NUM_THREADS + warpBase + lane] == digit)
//                mask |= (1u << lane);
//        }
        
//        uint rank = countbits(mask & ((1u << laneId) - 1));
//        uint groupSize = countbits(mask);
        
//        g_ranks[GTid.x] = rank;
        
//        // Warp histogram update only by lead thread
//        if (rank == 0)
//            InterlockedAdd(warpHist[warpId][digit], groupSize);
        
//        GroupMemoryBarrierWithGroupSync();
//    }
//}

//void BuildBlockHistogram(uint3 GTid)
//{
//    if (GTid.x < NUM_THREADS)
//    {
//        uint sum = 0;
//        for (uint w = 0; w < NUM_WARPS; w++)
//        {
//            sum += warpHist[w][GTid.x];
//        }
//        blockHist[GTid.x] = sum;
//    }

//    GroupMemoryBarrierWithGroupSync();
    
//    if (GTid.x == 0)
//    {
//        uint running = 0;
//        for (uint d = 0; d < NUM_THREADS; d++)
//        {
//            blockPrefix[d] = running;
//            running += blockHist[d];
//        }
//    }

//    GroupMemoryBarrierWithGroupSync();
//}

//uint Pack(uint status, uint value)
//{
//    return (status << 30) | (value & 0x3fffffff);
//}

//uint GetStatus(uint x)
//{
//    return x >> 30;
//}

//uint GetValue(uint x)
//{
//    return x & 0x3fffffff;
//}

//void PublishLocalCounts(uint tileId, uint3 GTid)
//{
//    for (uint d = GTid.x; d < NUM_THREADS; d += NUM_THREADS)
//    {
//        uint idx = tileId * NUM_THREADS + d;
//        scanState[idx] = Pack(1, blockHist[d]);
//    }

//    GroupMemoryBarrierWithGroupSync();
//}

//void ChainedScan(uint tileId, uint3 GTid)
//{
//    //for (uint d = GTid.x; d < NUM_THREADS; d += NUM_THREADS)
//    //{
//    //    uint prefix = 0;
//    //    int prev = int(tileId) - 1;

//    //    while (prev >= 0)
//    //    {
//    //        uint entry = scanState[prev * NUM_THREADS + d];
//    //        uint status = GetStatus(entry);
//    //        uint value = GetValue(entry);

//    //        if (status == 0)
//    //            continue;

//    //        prefix += value;

//    //        if (status == 2)
//    //            break;

//    //        prev--;
//    //    }

//    //    blockScanPrefix[d] = prefix;

//    //    uint inclusive = prefix + blockHist[d];
//    //    scanState[tileId * NUM_THREADS + d] = Pack(2, inclusive);
//    //}

//    GroupMemoryBarrierWithGroupSync();
//}

//[numthreads(256, 1, 1)]
//void main( uint3 DTid : SV_DispatchThreadID,
//        uint3 GTid : SV_GroupThreadID,
//        uint3 Gid : SV_GroupID)
//{
//    const uint tileSize = ITEM_PER_THREAD * NUM_THREADS;
    
//    //Thread 0 grabs a tile for the thread block
    
//    if (GTid.x == 0)
//        InterlockedAdd(tileCounter[0], 1, tileId);
    
//    GroupMemoryBarrierWithGroupSync();
 
//    BuildWarpHistogram(tileId, GTid);
//    BuildBlockHistogram(GTid);
    
//    PublishLocalCounts(tileId, GTid);
    
//    ChainedScan(tileId, GTid);
//}

#define NUM_BINS 256
#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint elementCount;
    uint dispatchGroupCount;
    uint digitPlace;
    uint sPadding[1];
}

StructuredBuffer<uint> exclusivePrefixSum : register(t0);
StructuredBuffer<uint> mortonCodeIn : register(t1);
StructuredBuffer<float3> positionIn : register(t2);
StructuredBuffer<float> massIn : register(t3);

RWStructuredBuffer<uint> scatterCounters : register(u0);
RWStructuredBuffer<uint> mortonCodeOut : register(u1);
RWStructuredBuffer<float3> positionOut : register(u2);
RWStructuredBuffer<float> massOut : register(u3);
RWStructuredBuffer<uint> debugBuffer : register(u4);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;

    if (index >= elementCount)
        return;
    
    //Get value
    uint key = mortonCodeIn[index];
    float3 position = positionIn[index];
    float mass = massIn[index];
    
    //Get radix
    uint shift = digitPlace * 8;
    uint digit = (key >> shift) & 0xFF;
    
    //Count prior matching digits
    uint rank = 0;
    
    for (uint i = 0; i < index; ++i)
    {
        uint other = mortonCodeIn[i];
        uint otherDigit = (other >> shift) & 0xFF;

        if (otherDigit == digit)
            rank++;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    //Get histogram index
    uint histIndex = digitPlace * NUM_BINS + digit;
    
    //Final destination
    uint dstIndex = exclusivePrefixSum[histIndex] + rank;

    //Scatter
    if (dstIndex < elementCount)
    {
        mortonCodeOut[dstIndex] = key;
        positionOut[dstIndex] = position;
        massOut[dstIndex] = mass;
        debugBuffer[dstIndex] = key;
    }
    
}