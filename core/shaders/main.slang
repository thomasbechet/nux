struct VOut
{
    float4 position : SV_Position;
    float3 normal;
    float2 uv;
}

struct Uniform
{
    float4x4 view;
    float4x4 proj;
    float4x4 model;
}

struct Vertex
{
    float3 position;
    float2 texcoord;
}

StructuredBuffer<float>  storage;
ConstantBuffer<Uniform>  uniform;
Texture2D                texture0;
SamplerState             sampler;

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
    Vertex vertex = pullVertex(vertexID);
    VOut output = {};
    float4 worldPos = mul(uniform.model, float4(vertex.position, 1));
    float4 viewPos  = mul(uniform.view, worldPos);
    output.position = mul(uniform.proj, viewPos);
    output.uv       = vertex.texcoord;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VOut input) : SV_Target
{
    return texture0.Sample(sampler, uint2(0));
}

