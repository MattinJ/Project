struct Input
{
    float3 position : POSITION;
};

struct Output
{
    float4 postion : SV_Position;
};

Output main(Input input )
{
    Output output;
    
    output.postion = float4(input.position, 1.0f);
    
    return output;
}