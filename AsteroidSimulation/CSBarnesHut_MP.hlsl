#define L 10
#define NUM_THREADS 256

cbuffer SettingsBuffer : register(b0)
{
    uint Count;
    float CellWidth;
    uint sPadding[2];
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

//Multipole data
RWStructuredBuffer<float3> centerOfMass : register(u0);
RWStructuredBuffer<float> combinedMass : register(u1);
RWStructuredBuffer<float> cellWidth : register(u2);

RWStructuredBuffer<uint> debugBuffer : register(u7);

[numthreads(NUM_THREADS, 1, 1)]
void main(uint3 GTid : SV_GroupThreadID,
          uint3 Gid : SV_GroupID)
{
    const float t_CellWidth = 17.f * 2.f;
    
    const uint index = Gid.x * NUM_THREADS + GTid.x;
    
    //debugBuffer[index] = keyBuffer[index];
    debugBuffer[index] = coBuffer[index];
}