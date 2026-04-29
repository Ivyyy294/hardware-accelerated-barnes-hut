
#define L 10

StructuredBuffer<uint> counterBuffer : register(t0);
StructuredBuffer<uint> kBuffer : register(t1);
RWStructuredBuffer<uint> nkBuffer : register(u0);
RWStructuredBuffer<uint> debugBuffer : register(u1);

uint GetWsKey(uint val, uint level)
{
    uint shiftInt = 3 * (L - level);
    uint prefixInt = val >> shiftInt;
    uint wsInt = (1 << (3 * level)) | prefixInt;
    return wsInt;
}

uint GetIndernalNodeId (uint k, uint l)
{
    uint baseCount = ((1 << (3 * l)) - 1) / 7;

    uint prefix = k >> 3 * (L - l);
    
    uint internalIndex = baseCount + prefix;
    return internalIndex;
}

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint kSize = counterBuffer[0];
    uint ni = (kSize - 1) / 7;
    
    uint index = DTid.x;

    if (index >= kSize)
        return;
    
    //Add leaf node to K'
    uint k = kBuffer[index];
    uint k1 = kBuffer[index + 1];
    uint keyRange = k1 - k;
    
    //A key at level l covers 8^L-l keys
    //L = 10
    uint octalBitsCount = floor(firstbithigh(keyRange) / 3);
    uint leafLevel = 10 - octalBitsCount;
    uint wsKey = GetWsKey(k, leafLevel);
    nkBuffer[ni + index] = wsKey;
    
    //Add internal nodes to K'
    
    //30-bit clz
    uint d = 29 - firstbithigh(k ^ k1);
    bool isInternalNode = d % 3 == 0;
    uint internalLevel = d / 3;
    
    
    if (!isInternalNode)
        return;
        
    uint wsInt = GetWsKey(k, internalLevel);
    uint internalIndex = GetIndernalNodeId(k, internalLevel);
    nkBuffer[internalIndex] = wsInt;
}