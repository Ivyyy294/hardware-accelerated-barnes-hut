//Globals
cbuffer Settings : register(b0)
{
    uint elementCount;
    uint sPadding[3];
}

cbuffer UnitCube : register(b1)
{
    float3 boundsMax;
    float3 boundsMin;
    double ucPadding;
};

RWStructuredBuffer<float3> positions : register(u0);
RWStructuredBuffer<uint> mortonCodes : register(u1);

uint Part1By2(uint val)
{
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

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    if (index >= elementCount)
        return;

    mortonCodes[index] = GetMortonKey(positions[index]);
}