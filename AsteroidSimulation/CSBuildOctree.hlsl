
cbuffer SettingsBuffer : register(b0)
{
    uint elementCount;
    uint dispatchGroupCount;
    uint sPadding[2];
}

StructuredBuffer<uint> mortonCodeIn : register(t0);
StructuredBuffer<uint> positionIn : register(t1);
StructuredBuffer<uint> massIn : register(t2);

int2 DetermineRange (int internalNodeIndex)
{
    int2 range;
    
    return range;
}

//binary search to find higest differing bit
//return index of last element with the bit set to 0
int FindSplit (int indexFirst, int indexLast)
{
    return 0;
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}