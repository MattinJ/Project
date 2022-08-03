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

cbuffer cubemapCb : register(b3)
{
    int cubeIndex;
    float3 pad3;
}


StructuredBuffer<LightBuffer> buffer : register(t0);

//Shadowmap depth textures
Texture2DArray shadowMapTexture : register(t1);

//gBuffers
Texture2D<float4> clipPositions : register(t2);
Texture2D<float4> normals : register(t3);
Texture2D<float4> diffuse : register(t4);
Texture2D<float4> worldPos : register(t5);
Texture2D<float4> ambient : register(t6);
Texture2D<float4> specular : register(t7);

//UAV
RWTexture2D<unorm float4> output : register(u0);
RWTexture2DArray<unorm float4> cubeMap : register(u1);

//Group size
#define size_x 32
#define size_y 32
#define size_z 1
#define shadow_bias 0.001f;

float shadowCalc(float3 worldPos, int index)
{
    //Shadow map
    
    //Light view space
    float4 lightViewPos = mul(float4(worldPos, 1.0f), lightVP);
    float2 projUV;
       
    projUV.x = lightViewPos.x / lightViewPos.w / 2.0f + 0.5f;
    projUV.y = -lightViewPos.y / lightViewPos.w / 2.0f + 0.5f;
        
    //Check if projUV are in light view
    if (projUV.x >= 0.0f && projUV.x <= 1.0f &&
        projUV.y >= 0.0f && projUV.y <= 1.0f)
    {
        //Sample the shadow map depth, we only need the Red value
        float lightDepthValue = shadowMapTexture.Load(float4(projUV * float2(1024, 1024), index, 0)).r;
        
        //Calculate the depth of light
        float pixeldepthValue = lightViewPos.z / lightViewPos.w;
        lightDepthValue = lightDepthValue - shadow_bias;
        
        //Compare depth
        if (lightDepthValue < pixeldepthValue)
        {
            return 0.3f;
        }
    }
    
    return 1.0f;
}

[numthreads(size_x, size_y, size_z)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 diffuseColor = diffuse[DTid.xy];
    float4 lightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 lightDir = float3(0.0f, 0.0f, 0.0f);
    float3 viewDir = normalize(cameraPos.xyz - worldPos[DTid.xy].xyz);
    float lightIntensity = 0;
    float3 reflection = float3(0.0f, 0.0f, 0.0f);
    float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float shadowFactor = 1.0f;
    
    //Set ambient to defualt color.
    float4 ambientColor = ambient[DTid.xy];
    
    for (uint i = 0; i < 1; i++)
    {
        if (buffer[i].lightType == 0) //Directional
        {
            lightDir = -buffer[i].lightDir;
            lightIntensity = saturate(dot(normals[DTid.xy].xyz, lightDir));
            
                
            if (lightIntensity > 0.0f)
            {
                //Calculate diffuse
                lightColor += diffuseColor * lightIntensity * buffer[i].lightColor;
            
                //Calculate specular
                reflection = normalize(reflect(-lightDir, normals[DTid.xy].xyz));
                specularIntensity = float4(specular[DTid.xy].xyz, 1.0f) * pow(saturate(dot(reflection, viewDir)), specular[DTid.xy].w);
                
                lightColor += specularIntensity;
            }
            
            //Calculate shadow
            shadowFactor = shadowCalc(worldPos[DTid.xy].xyz, i);
            
            lightColor *= shadowFactor;
       
        }
        else if (buffer[i].lightType == 1) //Spotlight
        {
            
            float3 pixelToLight = buffer[i].lightPos - worldPos[DTid.xy].xyz;
            float distance = length(pixelToLight);
            pixelToLight /= distance;
            
            if (distance > buffer[i].lightRange)
                continue;
            
            lightIntensity = saturate(dot(pixelToLight, normals[DTid.xy].xyz));
            lightDir = normalize(buffer[i].lightDir);
            
            if (lightIntensity > 0.0f)
            {
                //Calculate diffuse
                lightColor += diffuseColor * lightIntensity * buffer[i].lightColor;
                
                //Calculate cone cutoff
                lightColor *= pow(max(dot(-pixelToLight, buffer[i].lightDir), 0.0f), buffer[i].lightCone);
                
                //Calculate specular
                reflection = normalize(reflect(lightDir, normals[DTid.xy].xyz));
                lightColor += float4(specular[DTid.xy].xyz, 1.0f) * pow(saturate(dot(reflection, viewDir)), specular[DTid.xy].w);
                //lightColor += specularIntensity;
                
                //Calculate attuentions
                lightColor /= (buffer[i].lightAtt[0] + (buffer[i].lightAtt[1] * distance)) + (buffer[i].lightAtt[2] * (distance * distance));
     
            }
            
            shadowFactor = shadowCalc(worldPos[DTid.xy].xyz, i);
            lightColor *= shadowFactor;

        }
    }
    
    output[DTid.xy] = saturate(ambientColor + lightColor);
    cubeMap[float3(DTid.xy, cubeIndex)] = saturate(ambientColor + lightColor);
    //output[DTid.xy] = diffuse[DTid.xy];
    //output[DTid.xy] = float4(buffer[0].lightType, 0.0f, 0.0f, 1.0f);
    //output[DTid.xy] = float4(buffer[1].lightPos, 1.0f);

}