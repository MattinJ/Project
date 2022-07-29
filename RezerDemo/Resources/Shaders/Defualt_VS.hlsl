struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPos : POSITION1;
    float3 normal : NORMAL;
    float2 uv : UV;
};

cbuffer CameraBuffer : register(b0)
{
    matrix modelMatrix;
    matrix vpMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.worldPos = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(output.worldPos, vpMatrix);
    output.normal = mul(float4(input.normal, 0.0f), modelMatrix).xyz;
    output.uv = input.uv;
    
    return output;
}