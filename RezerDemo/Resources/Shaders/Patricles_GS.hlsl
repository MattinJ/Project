struct Input
{
    float4 postion : SV_Position;
};

struct Output
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : UV; 
};

cbuffer cbuf
{   
    matrix worldMatrix;
    matrix vpMatrix;
    float3 cameraPosition;
    float pad;
};

const static int nrOfVert = 6;

[maxvertexcount(nrOfVert)]
void main(point Input input[1], inout TriangleStream<Output> outputStream)
{
    const static float SIZE = 0.25f;
	
    //Calculate the direction
    float3 normalVector = normalize(cameraPosition - input[0].postion.xyz);
    
    float3 rightVector = normalize(cross(normalVector, float3(0.0f, 1.0f, 0.0f)));
    float3 upVector = cross(rightVector, normalVector);
    
    //rightVector = rightVector * SIZE;
    //upVector = upVector * SIZE;
        
    //Add dots so it form a quad.
    float3 vert[nrOfVert];
    vert[0] = input[0].postion.xyz - rightVector - upVector; //Bottom left
    vert[1] = input[0].postion.xyz - rightVector + upVector; //Top left
    vert[2] = input[0].postion.xyz + rightVector - upVector; //Bottom right
    
    vert[3] = input[0].postion.xyz - rightVector + upVector; //Top left
    vert[4] = input[0].postion.xyz + rightVector + upVector; //Top right
    vert[5] = input[0].postion.xyz + rightVector - upVector; //Bottom right
      
    //Texture uv
    float2 uv[nrOfVert];
    uv[0] = float2(0, 1);
    uv[1] = float2(0, 0);
    uv[2] = float2(1, 0);
    uv[3] = float2(1, 1);
    
    //Set and append
    Output output;
   
    for (int i = 0; i < nrOfVert; i++)
    {
        output.pos = mul(float4(vert[i], 1.0f), vpMatrix);
        output.worldPos = float4(vert[i], 1.0f);
        output.uv = uv[i];
        
        outputStream.Append(output);
        
    }
	
}