struct VOut
{
    float4 position : SV_Position;
    float2 uv;
}

Texture2D<uint> canvas;
Texture2D colormap;
SamplerState sampler0;
SamplerState sampler1;

float4 sampleColor(float2 uv)
{
    uint idx = canvas.Sample(sampler0, uv).r;
    return colormap.Sample(sampler1, float2(float(idx) / 255.0, 0));
}

float4 mix(float4 a, float4 b, float c)
{
    return b * c + a * (1 - c);
}

// https://www.shadertoy.com/view/4s3fDB
float4 bilinearFiltering(float2 uv, float2 res)
{
    float2 st = uv * res - 0.5;

    float2 iuv = floor(st);
    float2 fuv = fract(st);

    float4 a = sampleColor((iuv+float2(0.5,0.5)) / res);
    float4 b = sampleColor((iuv+float2(1.5,0.5)) / res);
    float4 c = sampleColor((iuv+float2(0.5,1.5)) / res);
    float4 d = sampleColor((iuv+float2(1.5,1.5)) / res);
    
    return mix(mix(a, b, fuv.x), mix(c, d, fuv.x), fuv.y);
}

float2 uvFiltering(float2 uv, float2 textureSize)
{
    float2 pixel = uv * textureSize;
    float2 seam = floor(pixel + 0.5);
    float2 dudv = fwidth(pixel);
    pixel = seam + clamp((pixel - seam) / dudv, -0.5, 0.5);
    return pixel / textureSize;
}

[shader("vertex")]
VOut vertexMain(
    uint vertexID : SV_VertexID,
)
{
    float x = float((vertexID & 1) << 2);
    float y = float((vertexID & 2) << 1);
    VOut output = {};
    output.position = float4(x - 1, y - 1, 0, 1);
    output.uv = float2(x, y) * 0.5;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VOut input) : SV_Target
{
    const float2 textureSize = float2(480, 300);
    float2 uv = float2(input.uv.x, 1 - input.uv.y);
    uv = uvFiltering(uv, textureSize);
    return bilinearFiltering(uv, textureSize);
}
