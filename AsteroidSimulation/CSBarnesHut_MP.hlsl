#include "CSUtils.hlsli"
#include "CSZOrder.hlsli"

#define L 10
#define NUM_THREADS 64

cbuffer SettingsBuffer : register(b0)
{
    uint pCount;
    float globallCellWidth;
    float sPadding[2];
}

//Particle data buffer
StructuredBuffer<uint> keyBuffer : register(t0);
StructuredBuffer<float3> posBuffer : register(t1);
StructuredBuffer<float> massBuffer : register(t2);

//Tree data buffer
StructuredBuffer<uint> kBuffer : register(t3);
StructuredBuffer<uint> nkBuffer : register(t4);
StructuredBuffer<uint> loBuffer : register(t5);
StructuredBuffer<uint> coBuffer : register(t6);
StructuredBuffer<uint> counterBuffer : register(t7);

//Multipole data
RWBuffer<float> cellMass : register(u0);
RWStructuredBuffer<float3> cellCom : register(u1);
RWStructuredBuffer<uint> liBuffer : register(u2);
RWStructuredBuffer<uint> lcBuffer : register(u3);
RWBuffer<float4> cellGeoData : register(u4);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint n = counterBuffer[1] + counterBuffer[0];
    
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    
    //Abort if index is out of bounds
    if (index >= n)
        return;
    
    //Abort if mass was already calculted in earlier pass
    if (cellMass[index] >= 0.f)
        return;

    //Init cell width
    const uint ws = nkBuffer[index];
    const uint level = GetTreeLevelFromWs(ws);
    const float cellWidth = globallCellWidth / (1 << level);

    //Init cell geo center
    float3 boundsMin = globallCellWidth * -0.5f;
    float3 boundsMax = globallCellWidth * 0.5f;
    cellGeoData[index] = float4(DecodeWSKey(ws, level, boundsMin, boundsMax), cellWidth);
    
    //Init li and lc buffer
    const uint prefix = ws - (1 << (3 * level));
    const uint range = 3 * (L - level);
    const uint begin = prefix << range;
    const uint end = (prefix + 1) << range;
    uint indexBegin = BinarySearch(begin, keyBuffer, pCount);
    uint indexEnd = BinarySearch(end, keyBuffer, pCount);
    uint count = indexEnd - indexBegin;
    liBuffer[index] = indexBegin;
    lcBuffer[index] = count;
    
    float4 localComMass = 0.f;
    
    //internal node
    if (coBuffer[index] > 0)
    {
        [unroll]
        for (uint i = 0; i < 8; ++i)
        {
            const uint childIndex = coBuffer[index] + i;
            const float iMass = cellMass[childIndex];

            //Early exit if one child is not ready yet
            if (iMass < 0.f)
                return;
            
            localComMass.w += iMass;
        
            const float3 iPos = cellCom[childIndex];
            localComMass.x += iPos.x * iMass;
            localComMass.y += iPos.y * iMass;
            localComMass.z += iPos.z * iMass;
        }
    }
    //Leaf node
    else
    {
        //max nCrit = 64 particle per node
        [unroll(64)]
        for (uint i = indexBegin; i < indexEnd; ++i)
        {
            const float iMass = massBuffer[i];
            localComMass.w += iMass;
        
            const float3 iPos = posBuffer[i];
            localComMass.x += iPos.x * iMass;
            localComMass.y += iPos.y * iMass;
            localComMass.z += iPos.z * iMass;
        }
    }
        
    if (localComMass.w > 0.f)
        localComMass.xyz = localComMass.xyz / localComMass.w;
        
    cellMass[index] = localComMass.w;
    cellCom[index] = localComMass.xyz;
}