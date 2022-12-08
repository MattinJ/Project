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
    float4 out_ambient : SV_Target3;
    float4 out_specular : SV_Target4;
};

cbuffer MaterialBuffer : register(b0)
{
    float specularPower;
    float3 pad3;
};

SamplerState objSamplerState : register(s0); //Our sampler from sampler state

Texture2D diffuseTexture : register(t0); //Our data from texture.
Texture2D ambientTexture : register(t1);
Texture2D specularTexture : register(t2);

PS_OUTPUT main(PS_INPUT input) : SV_Target
{
    PS_OUTPUT output;
    
    //Store position in RTV 1
    output.out_pos = input.in_pos;
    
    //Store normal in RTV 2
    input.in_normal = normalize(input.in_normal);
    output.out_normal = float4(input.in_normal, 0.0f);
    
    //Store color in RTV 3
    output.out_diffuse = diffuseTexture.Sample(objSamplerState, input.in_uv);;
    
    //Store world pos in RTV 4
    output.out_world_pos = input.in_world_pos;
    
    //Store ambient comp in RTV 5
    //output.out_ambient = ambientTexture.Sample(objSamplerState, input.in_uv);
    output.out_ambient = output.out_diffuse * 0.3;
    
    //Store specular comp in RTV 6
    output.out_specular = specularTexture.Sample(objSamplerState, input.in_uv);
    output.out_specular.w = 32.0f; //test
 
    return output;

}