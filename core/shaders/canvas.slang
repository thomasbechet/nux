struct VOut
{
    float4 position : SV_Position;
    float2 uv;
}

Texture2D canvas;
Texture2D colormap;
SamplerState sampler;

[shader("vertex")]
VOut vertexMain(
    uint vertexID : SV_VertexID,
)
{
    float x = float(vertexID & 1 << 2);
    float y = float(vertexID & 2 << 1);
    VOut output = {};
    output.position = float4(x - 1, y - 1, 0, 1);
    output.uv = float2(x, y) * 0.5;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VOut input) : SV_Target
{
    float2 uv = float2(input.uv.x, 1 - input.uv.y);
    uint idx = int(canvas.Sample(sampler, uv).r);
    return colormap.Sample(sampler, float2(float(idx) / 255, 0));
}