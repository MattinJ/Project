struct PS_INPUT
{
    float4 in_pos : SV_Position;
    float3 in_normal : NORMAL;
    float3 in_uv : UV;
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

SamplerState objSamplerState : register(s0); //Our sampler from sampler state

TextureCube cubeMap : register(t0);
Texture2D ambientTexture : register(t1);
Texture2D specularTexture : register(t2);

PS_OUTPUT main(PS_INPUT input) : SV_Target
{
    PS_OUTPUT output;
    
    float4 textureColor = cubeMap.Sample(objSamplerState, input.in_uv);
    
    //Store position in RTV 1
    output.out_pos = input.in_pos;
    
    //Store normal in RTV 2
    input.in_normal = normalize(input.in_normal);
    output.out_normal = float4(input.in_normal, 0.0f);
    
    //Store color in RTV 3
    output.out_diffuse = textureColor;
    
    //Store world pos in RTV 4
    output.out_world_pos = input.in_world_pos;
    
    //Store ambient comp in RTV 5
    output.out_ambient = textureColor * float4(0.3, 0.3, 0.3, 1.0f);
    
    //Store specular comp in RTV 6
    output.out_specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.out_specular.w = 32.0f; //test
 
    return output;

}