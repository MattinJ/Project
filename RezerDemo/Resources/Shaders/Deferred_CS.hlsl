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
    
    finalColor = colors[DTid.xy] * ambient;
        
    for (uint i = 0; i < 1; i++)
    {
        float3 lightToPixelVec = worldPos[DTid.xy].xyz - buffer[i].lightPos;
        float distance = length(lightToPixelVec);
        
        float3 finalAmbient = diffuse * ambient;
        
        float3 reflection = float3(0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        if (buffer[i].lightType == 0) //Directional
        {
            //lightVector = buffer[i].lightDir;
            //lightVector = normalize(-lightVector);
            
            finalColor += saturate(dot(buffer[i].lightDir, normals[DTid.xy].xyz) * diffuse * colors[DTid.xy]);
    
            //Specular
            reflection = normalize(reflect(lightVector, normals[DTid.xy].xyz));
       
            float3 viewVector = cameraPos - worldPos[DTid.xy].xyz;
            viewVector = normalize(viewVector);
        
            specularIntensity = float4(specularColor, 1.0f) * pow(saturate(dot(reflection, viewVector)), specularPower);
            
        }
        else if (buffer[i].lightType == 1) //Spotlight
        {
            if (distance > buffer[i].lightRange)
                continue;
         
            float lightPower = dot(lightVector, normals[DTid.xy].xyz);
            
            if (lightPower < 0.0f)
               continue;
           
            float3 lightDir = -normalize(buffer[i].lightDir);
            float3 diff = colors[DTid.xy] * diffuse * max(dot(normals[DTid.xy], lightVector), 0.0f)
            
            finalColor = colors[DTid.xy] * ambient;                      
            finalColor /= buffer[i].lightAtt[0] + buffer[i].lightAtt[1] * distance + buffer[i].lightAtt[2] * distance * distance;       
            finalColor *= pow(max(dot(lightVector, buffer[i].lightDir), 0.0f), buffer[i].lightCone);
           
        }
    
        finalColor = saturate(finalColor + specularIntensity.xyz);
    }
    
    output[DTid.xy] = float4(finalColor, 1.0f);
    //output[DTid.xy] = float4(finalColor, 1.0f) * float4(buffer[0].lightPos, 1.0f);
    //output[DTid.xy] = float4(buffer[0].lightType, 0.0f, 0.0f, 1.0f);
    //output[DTid.xy] = float4(buffer[0].lightPos, 1.0f);

}