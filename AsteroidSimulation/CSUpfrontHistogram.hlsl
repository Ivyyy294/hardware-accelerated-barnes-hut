#define NUM_BINS 256
#define NUM_DIGITS 4

//Globals
cbuffer Settings : register(b0)
{
    uint elementCount;
    uint sPadding[3];
}

RWStructuredBuffer<uint> mortonCodes : register(u0);
RWStructuredBuffer<uint> globalHistogram : register(u1);

groupshared uint localHistogram[NUM_DIGITS][NUM_BINS];

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    if (index >= elementCount)
        return;
    
    globalHistogram[index] = mortonCodes[index];
}