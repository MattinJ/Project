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


StructuredBuffer<LightBuffer> buffer : register(t0);

//Sampler
SamplerState detphSampler : register(s1);
//Shadowmap depth textures
Texture2D shadowMapTexture : register(t1);

//gBuffers
Texture2D<float4> clipPositions : register(t2);
Texture2D<float4> normals : register(t3);
Texture2D<float4> diffuse : register(t4);
Texture2D<float4> worldPos : register(t5);
Texture2D<float4> ambient : register(t6);
Texture2D<float4> specular : register(t7);

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
        float3 reflection = float3(0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        float3 shadowFactor = 1.0f;
        
        //Shadow map
    
        //Light view space
        float4 lightViewPos = mul(float4(worldPos[DTid.xy].xyz, 1.0f), lightVP);
        //lightViewPos /= lightViewPos.w;
        
        //if (lightViewPos.x >= -1.0f && lightViewPos.x <= 1.0f &&
        //    lightViewPos.y >= -1.0f && lightViewPos.y <= 1.0f &&
        //    lightViewPos.z >= 0.0f && lightViewPos.z <= 1.0f)
        //{
        //    lightViewPos.y *= -1.0f;
        //    lightViewPos.xy = (lightViewPos.xy + float2(1.0f, 1.0f)) * 0.5f;
            
        //    float2 invSize = 1.0f / 1024;
        //    float2 lightPosCorner = lightViewPos.xy * 1024;
        //    float2 floorLightPosCorner = floor(lightPosCorner);
        //    float2 fractLightPosCorner = frac(lightPosCorner);
            
        //    float bias = shadow_bias;
            
        //    Compare

        //    float lightDepth0 = shadowMapTexture.Sample(detphSampler, (floorLightPosCorner) * invSize) + bias >= lightViewPos.z ? 1.0f : 0.0f;
        //    float lightDepth1 = shadowMapTexture.Sample(detphSampler, (floorLightPosCorner + float2(1.0f, 0.0f)) * invSize) + bias >= lightViewPos.z ? 1.0f : 0.0f;
        //    float lightDepth2 = shadowMapTexture.Sample(detphSampler, (floorLightPosCorner + float2(0.0f, 1.0f)) * invSize) + bias >= lightViewPos.z ? 1.0f : 0.0f;
        //    float lightDepth3 = shadowMapTexture.Sample(detphSampler, (floorLightPosCorner + float2(1.0f, 1.0f)) * invSize) + bias >= lightViewPos.z ? 1.0f : 0.0f;

        //    Interpolate

        //    float edge0 = lerp(lightDepth0, lightDepth1, fractLightPosCorner.x);
        //    float edge1 = lerp(lightDepth2, lightDepth3, fractLightPosCorner.x);
        //    shadowFactor = lerp(edge0, edge1, fractLightPosCorner.y);
            

        //}
        
        float2 projUV;
       
        projUV.x = lightViewPos.x / lightViewPos.w / 2.0f + 0.5f;
        projUV.y = -lightViewPos.y / lightViewPos.w / 2.0f + 0.5f;
        
        //Check if projUV are in light view
        if (projUV.x >= 0.0f && projUV.x <= 1.0f &&
        projUV.y >= 0.0f && projUV.y <= 1.0f)
        {
            //Sample the shadow map depth, we only need the Red value
            float lightDepthValue = shadowMapTexture[projUV * float2(1024, 1024)].r;
        
            //Calculate the depth of light
            float pixeldepthValue = lightViewPos.z / lightViewPos.w;
            lightDepthValue = lightDepthValue - shadow_bias;
        
            //Compare depth
            if (lightDepthValue < pixeldepthValue)
            {
                shadowFactor = 0;
            }
    }
        
        
        if (buffer[i].lightType == 0) //Directional
        {
            float3 lightToPixelVec = buffer[i].lightDir;
            lightToPixelVec = normalize(-lightToPixelVec);
            
            finalColor += saturate(dot(buffer[i].lightDir, normals[DTid.xy].xyz) * buffer[i].lightDiffuse * diffuse[DTid.xy]);
    
            //Specular
            reflection = normalize(reflect(lightToPixelVec, normals[DTid.xy].xyz));
       
            float3 viewVector = cameraPos - worldPos[DTid.xy].xyz;
            viewVector = normalize(viewVector);
        
            specularIntensity = float4(specular[DTid.xy].xyz, 1.0f) * pow(saturate(dot(reflection, viewVector)), specular[DTid.xy].w);
            
        }
        else if (buffer[i].lightType == 1) //Spotlight
        {
            float3 lightToPixelVec = worldPos[DTid.xy].xyz - buffer[i].lightPos;
            float distance = length(lightToPixelVec);
             
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
    
        finalColor += saturate((finalColor + finalAmbient + specularIntensity.xyz) * shadowFactor);
    }
    
    output[DTid.xy] = float4(finalColor, 1.0f);
    //output[DTid.xy] = float4(shadowMapTexture[DTid.xy].r, 0.0f, 0.0f, 1.0f);
    //output[DTid.xy] = float4(buffer[0].lightType, 0.0f, 0.0f, 1.0f);
    //output[DTid.xy] = float4(buffer[1].lightPos, 1.0f);

}