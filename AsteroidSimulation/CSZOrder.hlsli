uint Part1By2(uint val)
{
    val &= 0x000003ff;
    val = (val ^ (val << 16)) & 0xff0000ff;
    val = (val ^ (val << 8)) & 0x0300f00f;
    val = (val ^ (val << 4)) & 0x030c30c3;
    val = (val ^ (val << 2)) & 0x09249249;
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
    const float3 boundsDim = boundsMax - boundsMin;
    value.x = (value.x + boundsMax.x) / boundsDim.x;
    value.y = (value.y + boundsMax.y) / boundsDim.y;
    value.z = (value.z + boundsMax.z) / boundsDim.z;
    return value;
}

uint GetMortonKey(float3 pos, float3 boundsMin, float3 boundsMax)
{
    float3 boundsDim;
    boundsDim.x = boundsMax.x - boundsMin.x;
    boundsDim.y = boundsMax.y - boundsMin.y;
    boundsDim.z = boundsMax.z - boundsMin.z;
    
    pos = MapToUnitCube(pos, boundsMin, boundsMax) * 1023;
    
    uint xx = Part1By2((uint) pos.x);
    uint yy = Part1By2((uint) pos.y);
    uint zz = Part1By2((uint) pos.z);
    
    return xx | (yy << 1) | (zz << 2);
}

float3 DecodeMortonKey(uint key, uint level, float3 boundsMin, float3 boundsMax)
{
    uint mask = 1 << (3 * level);
    uint prefix = key - mask;
    
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
