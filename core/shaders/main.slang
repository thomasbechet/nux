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

uniform uint drawID;

StructuredBuffer<float>   storage;
ConstantBuffer<Constants> constants;

Vertex pullVertex(uint idx)
{
    Vertex vertex = {}; 
    vertex.position = float3(
        storage[idx * 5 + 0],
        storage[idx * 5 + 1],
        storage[idx * 5 + 2]
    );
    vertex.texcoord = float2(
        storage[idx * 5 + 3],
        storage[idx * 5 + 4]
    );
    return vertex;
}

[shader("vertex")]
VOut vertexMain(
    uint vertexID : SV_VertexID,
)
{
    uint base = (vertexID / 3) * 3;
    Vertex v0 = pullVertex(vertexID);
    Vertex v1 = pullVertex(base + (vertexID + 1) % 3);
    Vertex v2 = pullVertex(base + (vertexID + 2) % 3);
    VOut output = {};
    float4 worldPos = mul(constants.model, float4(v0.position, 1));
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
