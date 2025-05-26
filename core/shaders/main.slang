struct VOut
{
    float4 position : SV_Position;
    float3 normal;
    float2 uv;
}

enum ShaderMode
{
    Main,
}

struct Uniform
{
    float4x4   view;
    float4x4   proj;
    ShaderMode mode;
}

StructuredBuffer<float>  storage_buffer;
ConstantBuffer<Uniform>  uniform_buffer;
Texture2D                texture0;
SamplerState             sampler;

float3 pullVertex(uint idx)
{

}

[shader("vertex")]
VOut vertexMain()
{
    VOut output = {};
    return output;
}

[shader("fragment")]
float4 fragmentMain(VOut input) : SV_Target
{
    return texture0.Sample(sampler, uint2(0));
}

