#include "CSUtils.hlsli"
#include "CSZOrder.hlsli"

#define NUM_THREADS 256
#define ITEM_PER_THREAD 8

//Globals
cbuffer SettingsBuffer : register(b0)
{
    uint elementCount;
    uint dispatchGroupCount;
    uint sPadding[2];
}

cbuffer UnitCube : register(b1)
{
    float3 boundsMax;
    float3 boundsMin;
    double ucPadding;
};

StructuredBuffer<float3> positions : register(t0);
RWStructuredBuffer<uint> mortonCodes : register(u0);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint tileSize = ITEM_PER_THREAD * NUM_THREADS;
    const uint groupId = Gid.x;
    const uint threadId = GTid.x;
    
    for (uint tileIndex = groupId; tileIndex * tileSize < elementCount; tileIndex += dispatchGroupCount)
    {
        const uint baseIndex = tileIndex * tileSize;

        for (uint i = 0; i < ITEM_PER_THREAD; ++i)
        {
            uint index = baseIndex + threadId + i * NUM_THREADS;
            
            if (index < elementCount)
                mortonCodes[index] = GetMortonKey(positions[index], -boundsMax, boundsMax);
        }
    }
}