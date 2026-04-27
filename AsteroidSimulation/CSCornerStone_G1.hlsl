cbuffer SettingsBuffer : register(b0)
{
    uint nCrit;
    uint sPadding[3];
}

StructuredBuffer<uint> kBuffer : register(t0);
StructuredBuffer<uint> nBuffer : register(t1);
StructuredBuffer<uint> outOBuffer : register(t2);
StructuredBuffer<uint> counterBuffer : register(t3);
RWStructuredBuffer<uint> oBuffer : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    uint kSize = counterBuffer[0];
    
    if (index > kSize)
        return;
    
    uint nVal = nBuffer[index];
    uint kVal = kBuffer[index];
    uint k1Val = kBuffer[index + 1];
    uint kDelta = k1Val - kVal;
    uint iDelta = (8 * kDelta);
    
    uint indexJ = (kVal % iDelta) / kDelta;
    uint indexFirstSibling = index - indexJ;
    uint indexLastSibling = indexFirstSibling + 8;
    
    bool split = nVal >= nCrit;
    bool firstOctant = kVal % iDelta == 0;
    bool consecutive = kBuffer[indexFirstSibling] + iDelta == kBuffer[indexLastSibling];
    
    uint combinedParticleCount = 0;
    
    for (uint i = indexFirstSibling; i <= indexLastSibling; ++i)
        combinedParticleCount += nBuffer[i];
    
    bool combine = !firstOctant && consecutive && combinedParticleCount < nCrit;

    if (split)
        oBuffer[index] = 8;
    else if (combine)
        oBuffer[index] = 0;
    else
        oBuffer[index] = 1;
}