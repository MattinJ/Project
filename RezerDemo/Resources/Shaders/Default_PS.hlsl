struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION1;
    float3 normal : NORMAL;
    float2 uv : UV;
};

cbuffer LightBuffer : register(b0)
{
    matrix vpLightMatrix;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}