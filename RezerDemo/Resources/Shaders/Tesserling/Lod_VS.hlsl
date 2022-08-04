struct Vertex
{
    float3 in_pos : POSITION;
    float3 in_normal : NORMAL;
    float2 in_uv : UV;
};

Vertex main(Vertex input)
{
    return input;
}