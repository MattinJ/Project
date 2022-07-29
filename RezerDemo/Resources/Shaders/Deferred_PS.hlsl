struct PS_INPUT
{
    float4 in_pos : SV_Position;
    float3 in_normal : NORMAL;
    float2 in_uv : UV;
    float4 in_world_pos : POSITION;
};

struct PS_OUTPUT
{
    float4 out_pos : SV_Target0;
    float4 out_normal : SV_Target1;
    float4 out_diffuse : SV_Target2;
    float4 out_world_pos : SV_Target2;
};

Texture2D objTexture : register(t0); //Our data from texture.
SamplerState objSamplerState : register(s0); //Our sampler from sampler state

PS_OUTPUT main(PS_INPUT input) : SV_Target
{
    PS_OUTPUT output;
    
    //Store position in RTV 1
    output.out_pos = input.in_pos;
    
    //Store normal in RTV 2
    input.in_normal = normalize(input.in_normal);
    output.out_normal = float4(input.in_normal, 0.0f);
    
    //Store color in RTV 3
    output.out_diffuse = objTexture.Sample(objSamplerState, input.in_uv);
    
    //Store world pos in RTV 4
    output.out_world_pos = input.in_world_pos;
    
    //Store specular comå in RTV 5
    
 
    return output;

}