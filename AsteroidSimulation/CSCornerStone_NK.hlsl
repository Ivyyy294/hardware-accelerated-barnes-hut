#define NUM_THREADS 256
#define L 10

StructuredBuffer<uint> kBuffer : register(t0);
RWStructuredBuffer<uint> nkBuffer : register(u0);
RWStructuredBuffer<uint> counterBuffer : register(u1);
RWStructuredBuffer<uint> debugBuffer : register(u2);

static const int MU[8] = { 0, -1, -2, -3, 3, 2, 1, 0 };

uint GetWsKey(uint val, uint level)
{
    uint shiftInt = 3 * (L - level);
    uint prefixInt = val >> shiftInt;
    uint wsInt = (1 << (3 * level)) | prefixInt;
    return wsInt;
}

uint GetOctalDigit (uint ki, uint t)
{
    // shift amount = 3 * (L - t)
    uint shift = 3u * (L - t);
    // extract 3 bits
    return (ki >> shift) & 7u;
}

int GetIndernalDelta (uint ki, uint d)
{
    uint max = (d / 3) + 1;
    
    int delta = 0;
    
    for (uint i = 0; i < max; ++i)
    {
        uint octalBit = GetOctalDigit(ki, (i + 1));
        delta += MU[octalBit];
    }
    
    return delta;
}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    uint kSize = counterBuffer[0];
    uint ni = (kSize - 1) / 7;
    
    uint index = Gid.x * NUM_THREADS + GTid.x;
    //debugBuffer[index] = ni;
    //return;

    if (index >= kSize)
        return;
    
    //debugBuffer[index] = 999;
    
    if (index == 0)
        counterBuffer[1] = ni;
    
    GroupMemoryBarrierWithGroupSync();
    
    //Add leaf node to K'
    uint k = kBuffer[index];
    uint k1 = kBuffer[index + 1];
    uint keyRange = k1 - k;
    
    //A key at level l covers 8^L-l keys
    //L = 10
    uint octalBitsCount = firstbithigh(keyRange) / 3;
    uint leafLevel = 10 - octalBitsCount;
    uint wsKey = GetWsKey(k, leafLevel);
    nkBuffer[ni + index] = wsKey;
    //debugBuffer[ni + index] = nkBuffer[ni + index];
    
    if (index >= kSize - 1)
        return;
    
    //Add internal nodes to K'
    //30-bit clz
    const uint d = 29 - firstbithigh(k ^ k1);
    const bool isInternalNode = d % 3 == 0;
    const uint internalLevel = d / 3;
    
    if (!isInternalNode)
        return;
    
    GroupMemoryBarrierWithGroupSync();
    
    const uint wsInt = GetWsKey(k, internalLevel);
    int internalKeyDelta = GetIndernalDelta(k, d);
    uint internalIndex = (index + internalKeyDelta) / 7;
    nkBuffer[internalIndex] = wsInt;
    GroupMemoryBarrierWithGroupSync();
    //debugBuffer[index] = internalIndex;
    
    //nkBuffer[0] = 1999;
}