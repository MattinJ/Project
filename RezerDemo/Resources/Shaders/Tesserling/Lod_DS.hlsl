struct DS_OUTPUT
{
    float4 out_pos : SV_Position;
    float3 out_normal : NORMAL;
    float2 out_uv : UV;
    float4 out_world_pos : POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float4 in_pos : SV_Position;
    float3 in_normal : NORMAL;
    float2 in_uv : UV;
    float4 in_world_pos : POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

cbuffer CBUF
{
    matrix worldMatrix;
    matrix vpMatrix;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 uvw : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> patch)
{
	DS_OUTPUT output;

    //Create plane
    float3 orgiWorldPostion = patch[0].in_world_pos * uvw.x + patch[1].in_world_pos * uvw.y + patch[2].in_world_pos * uvw.z;
   
    float3 worldplane1 = orgiWorldPostion - (dot(orgiWorldPostion - patch[0].in_world_pos.xyz, patch[0].in_normal)) * patch[0].in_normal;
    float3 worldplane2 = orgiWorldPostion - (dot(orgiWorldPostion - patch[1].in_world_pos.xyz, patch[1].in_normal)) * patch[1].in_normal;
    float3 worldplane3 = orgiWorldPostion - (dot(orgiWorldPostion - patch[2].in_world_pos.xyz, patch[2].in_normal)) * patch[2].in_normal;
    
    float3 finalWorldPos = worldplane1 * uvw.x + worldplane2 * uvw.y + worldplane3 * uvw.z;
    
    output.out_world_pos = float4(finalWorldPos, 1.0f);
    output.out_pos = mul(float4(finalWorldPos, 1.0f), vpMatrix);
  
    //Interpolera normal and uv
    output.out_normal = patch[0].in_normal * uvw.x + patch[1].in_normal * uvw.y + patch[2].in_normal * uvw.z;
    output.out_normal = normalize(output.out_normal);
    output.out_uv = patch[0].in_uv * uvw.x + patch[1].in_uv * uvw.y + patch[2].in_uv * uvw.z;
  
	return output;
}
