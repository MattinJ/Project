struct VS_INPUT
{
    float3 in_pos : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : UV;
};

struct VS_OUTPUT
{
    float4 out_pos : SV_Position;
    float3 out_normal : NORMAL;
    float2 out_uv : UV;
    float4 out_world_pos : POSITION;
};

cbuffer CBUF
{
    matrix worldMatrix;
    matrix vpMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    //Store only world position for light calulation
    float4 worldPostion = mul(float4(input.in_pos, 1.0f), worldMatrix);
    output.out_world_pos = worldPostion;
   
    //Calculate the position view and proj
    output.out_pos = mul(worldPostion, vpMatrix);
    
    //Calculate the normal against the world.
    output.out_normal = mul(input.in_normal, (float3x3) worldMatrix);
    output.out_normal = normalize(output.out_normal);
    
    //Store UV
    output.out_uv = input.in_uv;
    
    return output;
}