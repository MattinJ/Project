struct Input
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : UV;
};

cbuffer cubemapBuffer : register(b0)
{
    int cubeIndex;
    float3 pad3;
}

SamplerState objSamplerState : register(s0);
Texture2D objTexture : register(t0);

RWTexture2DArray<unorm float4> cubeMap : register(u0);

float4 main(Input input) : SV_TARGET
{
    float4 color = objTexture.Sample(objSamplerState, input.uv);
    
    clip(color.r - 1.0);
    
    return color;
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}