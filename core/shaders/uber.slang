#include "common.slang"

struct VOut
{
    float4 position : SV_Position;
    float3 normal;
    float2 uv;
}

struct Vertex
{
    float3 position;
    float2 texcoord;
}

struct Batch
{
    uint firstVertex;
    uint firstTransform;
    uint hasTexture;
}

Texture2D texture0;
SamplerState sampler0;
ConstantBuffer<Constants> constants;
StructuredBuffer<float> vertices;
StructuredBuffer<Batch> batches;
StructuredBuffer<float4x4> transforms;

Vertex pullVertex(uint idx)
{
    Vertex vertex = {};
    vertex.position = float3(
        vertices[idx * 5 + 0],
        vertices[idx * 5 + 1],
        vertices[idx * 5 + 2],
    );
    vertex.texcoord = float2(
        vertices[idx * 5 + 3],
        vertices[idx * 5 + 4],
    );
    return vertex;
}

[shader("vertex")]
VOut vertexMain(
    uint vertexIndex : SV_VertexID,
    uniform uint batchIndex,
)
{
    Batch batch = batches[batchIndex];
    uint base = batch.firstVertex + (vertexIndex / 3) * 3;
    Vertex v0 = pullVertex(base + (vertexIndex + 0) % 3);
    Vertex v1 = pullVertex(base + (vertexIndex + 1) % 3);
    Vertex v2 = pullVertex(base + (vertexIndex + 2) % 3);
    float4x4 transform = transforms[batch.firstTransform];
    VOut output = {};
    float4 worldPos = mul(transform, float4(v0.position, 1));
    float4 viewPos  = mul(constants.view, worldPos);
    output.position = mul(constants.proj, viewPos);
    output.uv       = v0.texcoord;
    // output.normal   = normalize(cross(
    //     v2.position - v1.position, 
    //     v0.position - v1.position));
    return output;
}

[shader("fragment")]
float4 fragmentMain(
    VOut input,
    uniform uint batchIndex,
) : SV_Target
{
    Batch batch = batches[batchIndex];
    if (batch.hasTexture != 0)
    {
        return texture0.Sample(sampler0, input.uv);
    }
    else
    {
        return float4(input.uv, 0, 1);
    }
}
