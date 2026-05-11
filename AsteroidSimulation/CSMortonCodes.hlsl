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
RWStructuredBuffer<uint> debugBuffer : register(u1);

uint Part1By2(uint val)
{
    val &= 0x000003ff;
    val = (val * 0x00010001u) & 0xFF0000FFu;
    val = (val * 0x00000101u) & 0x0F00F00Fu;
    val = (val * 0x00000011u) & 0xC30C30C3u;
    val = (val * 0x00000005u) & 0x49249249u;
    return val;
}

float3 MapToUnitCube(float3 value)
{
    value.x = (value.x - boundsMin.x) / (boundsMax.x - boundsMin.x);
    value.y = (value.y - boundsMin.y) / (boundsMax.y - boundsMin.y);
    value.z = (value.z - boundsMin.z) / (boundsMax.z - boundsMin.z);
    return value;
}

uint GetMortonKey(float3 pos)
{
    pos = MapToUnitCube(pos) * 1023;

    //ToDo: check why used in paper
    //pos.x = min(max(pos.x, 1024.f), 1023.f);
    //pos.y = min(max(pos.y, 1024.f), 1023.f);
    //pos.z = min(max(pos.z, 1024.f), 1023.f);
    
    uint xx = Part1By2((uint) pos.x);
    uint yy = Part1By2((uint) pos.y);
    uint zz = Part1By2((uint) pos.z);
    
    return xx ^ (yy << 1) ^ (zz << 2);
}

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
                mortonCodes[index] = GetMortonKey(positions[index]);
        }
    }

    //debugBuffer[index] = positions[index];
}