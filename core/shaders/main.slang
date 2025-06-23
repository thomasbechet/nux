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

ConstantBuffer<Constants> constants;
StructuredBuffer<float> vertices;
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
    uniform uint vertexFirst,
    uniform uint transformIndex,
)
{
    uint base = vertexFirst + (vertexIndex / 3) * 3;
    Vertex v0 = pullVertex(vertexIndex);
    Vertex v1 = pullVertex(base + (vertexIndex + 1) % 3);
    Vertex v2 = pullVertex(base + (vertexIndex + 2) % 3);
    float4x4 transform = transforms[transformIndex];
    VOut output = {};
    float4 worldPos = mul(transform, float4(v0.position, 1));
    float4 viewPos  = mul(constants.view, worldPos);
    output.position = mul(constants.proj, viewPos);
    output.uv       = v0.texcoord;
    output.normal   = normalize(cross(
        v2.position - v1.position, 
        v0.position - v1.position));
    return output;
}

[shader("fragment")]
float4 fragmentMain(VOut input) : SV_Target
{
    float d = dot(input.normal, normalize(float3(cos(constants.time), 1, sin(constants.time))));
    return max(d, 0.5) * float4(1, 0, 0, 1);
}
