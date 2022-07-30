//Constant buffers
struct LightBuffer
{
    float3 lightPos;
    float lightRange;
    float3 lightDir;
    float lightCone;
    float4 lightDiffuse;
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
    float4 test1;
    float4 test2;
    float3 test3;
    float test4;
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
Texture2D<float4> diffuse : register(t3);
Texture2D<float4> worldPos : register(t4);
Texture2D<float4> ambient : register(t5);
Texture2D<float4> specular : register(t6);

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
    
    float3 finalAmbient = ambient[DTid.xy] * diffuse[DTid.xy];
        
    for (uint i = 0; i < 1; i++)
    {
        float3 lightToPixelVec = worldPos[DTid.xy].xyz - buffer[i].lightPos;
        float distance = length(lightToPixelVec);
        
        float3 reflection = float3(0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        if (buffer[i].lightType == 0) //Directional
        {
            //lightToPixelVec = buffer[i].lightDir;
            //lightToPixelVec = normalize(-lightToPixelVec);
            
            finalColor += saturate(dot(buffer[i].lightDir, normals[DTid.xy].xyz) * buffer[i].lightDiffuse * diffuse[DTid.xy]);
    
            //Specular
            reflection = normalize(reflect(lightToPixelVec, normals[DTid.xy].xyz));
       
            float3 viewVector = cameraPos - worldPos[DTid.xy].xyz;
            viewVector = normalize(viewVector);
        
            specularIntensity = float4(specular[DTid.xy].xyz, 1.0f) * pow(saturate(dot(reflection, viewVector)), specular[DTid.xy].w);
            
        }
        else if (buffer[i].lightType == 1) //Spotlight
        {
            
             
            if (distance > buffer[i].lightRange)
            continue;
            
            lightToPixelVec = normalize(lightToPixelVec);
            
            float3 normalLightDir = normalize(buffer[i].lightDir);
            
            float lightDiffuse = max(dot(-lightToPixelVec, normals[DTid.xy].xyz), 0);
            
            float scalar = dot(lightToPixelVec, normalLightDir);

            float coneCufOff = 0;
            
            if(scalar >= 0.9)
            {
                coneCufOff = 1;
            }
            
            if (lightDiffuse < 0.0f)
                continue;
            
            //Specular
            reflection = normalize(reflect(lightToPixelVec, normals[DTid.xy].xyz));
       
            float3 viewVector = cameraPos - worldPos[DTid.xy].xyz;
            viewVector = normalize(viewVector);
        
            specularIntensity = float4(specular[DTid.xy].xyz, 1.0f) * pow(max(dot(reflection, viewVector), 0.0f), specular[DTid.xy].w) * coneCufOff;
            
            //Spotlight
            lightToPixelVec = normalize(lightToPixelVec);
            
            finalColor += diffuse[DTid.xy] * lightDiffuse * lightDiffuse * coneCufOff;
                      
            finalColor /= buffer[i].lightAtt[0] + buffer[i].lightAtt[1] * distance + buffer[i].lightAtt[2] * distance * distance;       
            finalColor *= pow(max(dot(lightToPixelVec, buffer[i].lightDir), 0.0f), buffer[i].lightCone);
            
        }
    
        finalColor = saturate(finalColor + finalAmbient + specularIntensity.xyz );
    }
    
    output[DTid.xy] = float4(finalColor, 1.0f);
    //output[DTid.xy] = specular[DTid.xy];
    //output[DTid.xy] = float4(buffer[0].lightType, 0.0f, 0.0f, 1.0f);
    //output[DTid.xy] = float4(buffer[0].lightPos, 1.0f);

}