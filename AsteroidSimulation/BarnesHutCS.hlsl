StructuredBuffer<float3> position : register(t0);
StructuredBuffer<float> mass : register(t1);

RWStructuredBuffer<int> OutputData : register(u0);

uint GetMortonCode(uint val)
{
    val &= 0x000003ff;
    val = (val ^ (val << 16)) & 0xff0000ff;
    val = (val ^ (val << 8)) & 0x0300f00f;
    val = (val ^ (val << 4)) & 0x030c30c3;
    val = (val ^ (val << 2)) & 0x09249249;
    return val;
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
      
    int test = GetMortonCode(255);

    //int test = InputData[index];
    // bounds safety (important in real use)
    // assume you pass N via constant buffer if needed
    OutputData[index] = mass[index] * 1024;
}