uint Part1By2(uint val)
{
    val &= 0x000003ff;
    val = (val * 0x00010001u) & 0xFF0000FFu;
    val = (val * 0x00000101u) & 0x0F00F00Fu;
    val = (val * 0x00000011u) & 0xC30C30C3u;
    val = (val * 0x00000005u) & 0x49249249u;
    return val;
}

uint Compact1By2(uint x)
{
    x &= 0x09249249;
    x = (x ^ (x >> 2)) & 0x030c30c3;
    x = (x ^ (x >> 4)) & 0x0300f00f;
    x = (x ^ (x >> 8)) & 0xff0000ff;
    x = (x ^ (x >> 16)) & 0x000003ff;
    return x;
}

float3 MapToUnitCube(float3 value, float3 boundsMin, float3 boundsMax)
{
    value.x = (value.x - boundsMin.x) / (boundsMax.x - boundsMin.x);
    value.y = (value.y - boundsMin.y) / (boundsMax.y - boundsMin.y);
    value.z = (value.z - boundsMin.z) / (boundsMax.z - boundsMin.z);
    return value;
}

uint GetMortonKey(float3 pos, float3 boundsMin, float3 boundsMax)
{
    pos = MapToUnitCube(pos, boundsMin, boundsMax) * 1023;
    
    uint xx = Part1By2((uint) pos.x);
    uint yy = Part1By2((uint) pos.y);
    uint zz = Part1By2((uint) pos.z);
    
    return xx | (yy << 1) | (zz << 2);
}

float3 DecodeWSKey(uint key, uint level, float3 boundsMin, float3 boundsMax)
{
    uint mask = 1 << (3 * level);
    uint prefix = mask ^ key;
    
    const float3 boundsDim = boundsMax - boundsMin;
    
    float cOffset = 1.f / (1 << (level + 1));
    float3 lc = 0.f;
    lc.x = Compact1By2(prefix);
    lc.y = Compact1By2(prefix >> 1);
    lc.z = Compact1By2(prefix >> 2);
    
    lc /= (1 << level);
    lc += cOffset;
    
    float3 wc = boundsMin + (lc * boundsDim);
    return wc;
}