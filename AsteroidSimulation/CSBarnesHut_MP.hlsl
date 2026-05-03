#include "CSUtils.hlsli"

#define L 10
#define NUM_THREADS 256

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
RWStructuredBuffer<float3> centerOfMass : register(u0);
RWStructuredBuffer<float> combinedMass : register(u1);
RWStructuredBuffer<uint> liBuffer : register(u2);
RWStructuredBuffer<uint> lcBuffer : register(u3);

RWStructuredBuffer<uint> debugBuffer : register(u7);

void CallculateLeafNodes(const uint index, const uint nl, const uint ni, const uint pCount)
{
    for (int i = 0; i < nl; i += NUM_THREADS)
    {
        uint nlIndex = i + index;
        
        if (nlIndex < nl)
        {
            float mass = 0.f;
            float3 center;
    
            const uint kVal = kBuffer[index];
            const uint k1Val = kBuffer[index + 1];
        
            uint indexBegin = BinarySearch(kVal, keyBuffer, pCount);
            uint indexLast = BinarySearch(k1Val, keyBuffer, pCount);
        
            for (uint i = indexBegin; i < indexLast; ++i)
            {
                const float iMass = massBuffer[i];
                mass += iMass;
        
                const float3 iPos = posBuffer[i];
                center.x += iPos.x * iMass;
                center.y += iPos.y * iMass;
                center.z += iPos.z * iMass;
            }
    
            const uint destIndex = ni + nlIndex;
            centerOfMass[destIndex] = center / mass;
            combinedMass[destIndex] = mass;
            liBuffer[destIndex] = indexBegin;
            lcBuffer[destIndex] = indexLast - indexBegin;
        }
    }
}

void CallculateInternalNodes(const uint index, const uint ni, const uint n)
{
    int startLevel = GetTreeLevelFromWs(nkBuffer[ni]);
    
    //ToDo: improve traversal!
    //Traverse per level upward
    for (int i = startLevel; i >= 0; --i)
    {
        const uint startIndex = loBuffer[i];
        const uint endIndex = loBuffer[i+1];
        const uint niIndex = startIndex + index;
        
        if (niIndex < endIndex)
        {
            const uint indexFirstChild = coBuffer[niIndex];
            
            if (indexFirstChild < n)
            {
                const uint indexLastChild = coBuffer[niIndex + 1];
                float mass = 0.f;
                float3 center;
                
                for (uint j = indexFirstChild; j < indexLastChild; ++j)
                {
                    const float iMass = combinedMass[j];
                    mass += iMass;
                    
                    const float3 iPos = posBuffer[i];
                    center.x += iPos.x * iMass;
                    center.y += iPos.y * iMass;
                    center.z += iPos.z * iMass;
                }
                
                centerOfMass[niIndex] = center / mass;
                combinedMass[niIndex] = mass;
            }
        }
        
        GroupMemoryBarrierWithGroupSync();
    }
}

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    const uint ni = counterBuffer[1];
    const uint nl = counterBuffer[0];
    const uint n = ni + nl;
    
    CallculateLeafNodes(index, nl, ni, pCount);
    
    GroupMemoryBarrierWithGroupSync();
    
    CallculateInternalNodes(index, ni, n);
    //debugBuffer[index] = combinedMass[index];
    //debugBuffer[index] = pCount;
    //debugBuffer[index] = kBuffer[index];
    //debugBuffer[index] = loBuffer[index];
    //debugBuffer[index] = coBuffer[index];
    //debugBuffer[index] = nl;
    //debugBuffer[index] = nkBuffer[index];
    //debugBuffer[index] = liBuffer[ index];
    //debugBuffer[index] = lcBuffer[index];
    return;
}