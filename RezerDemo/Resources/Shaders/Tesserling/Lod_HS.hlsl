struct VS_CONTROL_POINT_OUTPUT
{
    float4 in_pos : SV_Position;
    float3 in_normal : NORMAL;
    float2 in_uv : UV;
    float4 in_world_pos : POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 out_pos : SV_Position;
    float3 out_normal : NORMAL;
    float2 out_uv : UV;
    float4 out_world_pos : POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

cbuffer camera : register(b0)
{
    float3 cameraPos;
    float pad1;
    float3 objetPos;
    float pad2;
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_CONTROL_POINT_OUTPUT, 3> inputPatch, uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;

    const int maxDetails = 10;
    const int minDetails = 1;
    
    //räkna ut dessa konstantera beroende på kameran
    float tessFactor = max(maxDetails - (length(objetPos - cameraPos)), minDetails);
     
	for (int i = 0; i < 3; ++i)
    {
        output.EdgeTessFactor[i] = tessFactor; //umber of  outer edges
    }
	
    output.InsideTessFactor = tessFactor; // number of inner triangles
	
    return output;
}

[domain("tri")]									//What we work with
[partitioning("integer")]						//What partions we work with
[outputtopology("triangle_cw")]					//Topology output "counter clock-wise
[outputcontrolpoints(3)]						//How many control points we output.
[patchconstantfunc("CalcHSPatchConstants")] //Set function for tesselering
HS_CONTROL_POINT_OUTPUT main( InputPatch<VS_CONTROL_POINT_OUTPUT, 3> inputPatch, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	return inputPatch[i];
}
