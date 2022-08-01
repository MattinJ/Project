struct Input
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : UV;
};

SamplerState objSamplerState : register(s0);
Texture2D objTexture : register(t0); 

float4 main(Input input) : SV_TARGET
{
    //float4 color = objTexture.Sample(objSamplerState, input.uv);
    
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}