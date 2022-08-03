cbuffer cbuf
{
    float3 startPosition;
    float pad1;
    float deltaTime;
    float randomTime;
    float speed;
    float pad2;
};

RWBuffer<float> particlePosition : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    particlePosition[DTid.x * 4 + 1] -= particlePosition[DTid.x * 4 + 3] * 0.1; //Fallspeed
    
    if (particlePosition[DTid.x * 4 + 1] < 0.0)
        particlePosition[DTid.x * 4 + 1] = 10;

}