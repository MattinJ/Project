cbuffer cbuf
{
    float3 startPosition;
    float time;  
};

RWBuffer<float> particlePosition : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    particlePosition[DTid.x * 3] = startPosition + cos(0.001f + DTid.x) * cos(time) * 4;
    particlePosition[DTid.x * 3 + 1] = startPosition + sin(0.001f + DTid.x) * sin(time) * 4;
    
}