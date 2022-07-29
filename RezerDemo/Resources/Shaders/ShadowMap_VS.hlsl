cbuffer CameraBuffer : register(b0)
{
    matrix worldMatrix;
    matrix vpMatrix;
};

struct VS_INPUT
{
    float3 in_pos : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : UV;
};

struct Output
{
    float4 position : SV_Position;
};

Output main(VS_INPUT input)
{
    Output output;
    
    //Store only world position for light calulation
    float4 worldPostion = mul(float4(input.in_pos, 1.0f), worldMatrix);
   
    //Calculate the position view and proj
    output.position = mul(worldPostion, vpMatrix);
    
    return output;
}