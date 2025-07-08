#include "common.slang"

struct VOut
{
    float4 position : SV_Position;
    float2 uv;
}

ConstantBuffer<Constants> constants;
StructuredBuffer<uint32_t> quads;
Texture2D texture0;
SamplerState sampler0;

struct UniformData
{
    uint mode;
    uint firstQuad;
    uint atlasWidth;
    uint atlasHeight;
}

[shader("vertex")]
VOut vertexMain(
    uint vertexIndex : SV_VertexID,
    uniform UniformData data,
)
{
    const float2 offsets[6] = {
        { 0, 0 },
        { 0, 1 },
        { 1, 1 },
        { 1, 1 },
        { 1, 0 },
        { 0, 0 }
    };

    // Extract quad data
    uint index = data.firstQuad + vertexIndex / 6;
    uint32_t pos  = quads[index * 3 + 0];
    uint32_t tex  = quads[index * 3 + 1];
    uint32_t size = quads[index * 3 + 2];

    // Decode quad data
    float2 vertex_pos = float2(float(pos & 0xffffu), float(pos >> 16u)); 
    float2 vertex_tex = float2(float(tex & 0xffffu), float(tex >> 16u));
    float2 vertex_size = float2(float(size & 0xffffu), float(size >> 16u));

    // Compute vertex offset based on the vertex index
    float2 vertex_offset = offsets[vertexIndex % 6];

    // Apply offset and normalize
    float2 position = (vertex_pos + vertex_size * vertex_offset) / constants.screen_size; 
    position.y = 1 - position.y;
    float2 uv = floor(vertex_tex + vertex_size * vertex_offset) / float2(data.atlasWidth, data.atlasHeight);

    // Store output 
    VOut output = {};
    // output.position = float4(position * 2 - 1, depth, 1);
    output.position = float4(position * 2 - 1, 0, 1);
    output.uv       = uv;
    return output;
}

[shader("fragment")]
float4 fragmentMain(
    VOut input, 
    uniform UniformData data,
) : SV_Target
{
    if (data.mode == 1)
    {
        return texture0.Sample(sampler0, input.uv);
    }
    return float4(1, 1, 1, 1);
}
