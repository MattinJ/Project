//Constant buffers
struct LightBuffer
{
    float3 lightPos;
    float lightRange;
    float3 lightDir;
    float lightCone;
    float4 lightColor;
    float3 lightAtt;
    int lightType;
};

cbuffer LightMatirx : register(b0)
{
    matrix lightWorld;
    matrix lightVP;
}

cbuffer MaterialBuffer : register(b1)
{
    float4 ambient;
    float4 diffuse;
    float3 specularColor;
    float specularPower;
};

cbuffer camera : register(b2)
{
    float3 cameraPos;
    float pad;
};

//Sampler
//Texture2D depthMapTexture : register(t0);

StructuredBuffer<LightBuffer> buffer : register(t0);

//SRV
Texture2D<float4> clipPositions : register(t1);
Texture2D<float4> normals : register(t2);
Texture2D<float4> colors : register(t3);
Texture2D<float4> worldPos : register(t4);

//UAV
RWTexture2D<unorm float4> output : register(u0);

//Group size
#define size_x 32
#define size_y 32
#define size_z 1
#define shadow_bias 0.001f;
    
[numthreads(size_x, size_y, size_z)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 finalAmbient = ambient * colors[DTid.xy];
        
    for (uint i = 0; i < 1; i++)
    {
        float3 lightToPixelVec = worldPos[DTid.xy].xyz - buffer[i].lightPos;
        float distance = length(lightToPixelVec);
        
        float3 reflection = float3(0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        if (buffer[i].lightType == 0) //Directional
        {
            lightToPixelVec = buffer[i].lightDir;
            lightToPixelVec = normalize(-lightToPixelVec);
            
            finalColor += saturate(dot(buffer[i].lightDir, normals[DTid.xy].xyz) * diffuse * colors[DTid.xy]);
    
            //Specular
            reflection = normalize(reflect(lightToPixelVec, normals[DTid.xy].xyz));
       
            float3 viewVector = cameraPos - worldPos[DTid.xy].xyz;
            viewVector = normalize(viewVector);
        
            specularIntensity = float4(specularColor, 1.0f) * pow(saturate(dot(reflection, viewVector)), specularPower);
            
        }
        else if (buffer[i].lightType == 1) //Spotlight
        {
            if (distance > buffer[i].lightRange)
                continue;
            
            float lightPower = dot(-lightToPixelVec, normals[DTid.xy].xyz);
            
            if (lightPower < 0.0f)
                continue;
            
            //Specular
            reflection = normalize(reflect(lightToPixelVec, normals[DTid.xy].xyz));
       
            float3 viewVector = cameraPos - worldPos[DTid.xy].xyz;
            viewVector = normalize(viewVector);
        
            specularIntensity = float4(specularColor, 1.0f) * pow(saturate(dot(reflection, viewVector)), specularPower);
            
            lightToPixelVec /= distance;
            
            finalColor += colors[DTid.xy] * diffuse;
                      
            finalColor /= buffer[i].lightAtt[0] + buffer[i].lightAtt[1] * distance + buffer[i].lightAtt[2] * distance * distance;       
            finalColor *= pow(max(dot(lightToPixelVec, buffer[i].lightDir), 0.0f), buffer[i].lightCone);
            
        }
    
        finalColor = saturate(finalColor + finalAmbient + specularIntensity.xyz );
    }
    
    output[DTid.xy] = float4(finalColor, 1.0f);
    //output[DTid.xy] = float4(finalColor, 1.0f) * float4(buffer[0].lightPos, 1.0f);
    //output[DTid.xy] = float4(buffer[0].lightType, 0.0f, 0.0f, 1.0f);
    //output[DTid.xy] = float4(buffer[0].lightPos, 1.0f);

}