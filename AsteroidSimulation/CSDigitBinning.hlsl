#define ITEM_PER_THREAD 8
#define NUM_THREADS 256

RWStructuredBuffer<uint> tileCounter : register(u0);

//could be read only
RWStructuredBuffer<uint> exclusivePrefixSum : register(u1);

RWStructuredBuffer<uint> mortonCodes : register(u2);
RWStructuredBuffer<float3> position : register(u3);
RWStructuredBuffer<float> mass : register(u4);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID,
        uint3 GTid : SV_GroupThreadID,
        uint3 Gid : SV_GroupID)
{
    const uint tileSize = ITEM_PER_THREAD * NUM_THREADS;
    uint tileId;
    
    InterlockedAdd(tileCounter[0], 1, tileId);
    
    const uint baseIndex = tileId * tileSize;

    for (uint i = baseIndex + GTid.x; i < baseIndex + tileSize; i += NUM_THREADS)
    {
        const uint index = baseIndex + GTid.x + i * NUM_THREADS;
    }
}