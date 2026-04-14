StructuredBuffer<int> InputData : register(t0);
RWStructuredBuffer<int> OutputData : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;

    int test = InputData[index];
    // bounds safety (important in real use)
    // assume you pass N via constant buffer if needed
    OutputData[index] = InputData[index] + 4;
}