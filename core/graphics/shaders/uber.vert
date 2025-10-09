#version 450

#define VERTEX_POSITION uint(1 << 0)
#define VERTEX_TEXCOORD uint(1 << 1)
#define VERTEX_COLOR    uint(1 << 2)

struct Constants
{
    mat4 view;
    mat4 proj;
    uvec2 screenSize;
    float time;
};

struct Batch
{
    uint firstVertex;
    uint firstTransform;
    uint hasTexture;
    uint attributes;
};

struct Vertex
{
    vec3 position;
    vec2 texcoord;
    vec3 color;
};

layout(binding = 2, std140) uniform ConstantBlock
{
    Constants constants;
};
layout(binding = 3, std430) readonly buffer BatchBlock
{
    Batch batches[];
};
layout(binding = 4, std430) readonly buffer VertexBlock
{
    float vertices[];
};
layout(binding = 5, std430) readonly buffer TransformBlock
{
    mat4 transforms[];
};

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

uniform uint batchIndex;

Vertex pullVertex(uint idx, uint attributes)
{
    uint position = 0;
    uint texcoord = 0;
    uint color = 0;
    uint size = 0;
    if ((attributes & VERTEX_POSITION) != 0)
    {
        position = size;
        size += 3;
    }
    if ((attributes & VERTEX_TEXCOORD) != 0)
    {
        texcoord = size;
        size += 2;
    }
    if ((attributes & VERTEX_COLOR) != 0)
    {
        color = size;
        size += 3;
    }
    uint offset = idx * size;
    Vertex vertex;
    vertex.position = vec3(
            vertices[offset + position + 0],
            vertices[offset + position + 1],
            vertices[offset + position + 2]
        );
    vertex.texcoord = vec2(
            vertices[offset + texcoord + 0],
            vertices[offset + texcoord + 1]
        );
    vertex.color = vec3(
            vertices[offset + color + 0],
            vertices[offset + color + 1],
            vertices[offset + color + 2]
        );
    return vertex;
}

void main()
{
    Batch batch = batches[batchIndex];
    uint base = batch.firstVertex + (gl_VertexID / 3) * 3;
    Vertex v0 = pullVertex(base + (gl_VertexID + 0) % 3, batch.attributes);
    Vertex v1 = pullVertex(base + (gl_VertexID + 1) % 3, batch.attributes);
    Vertex v2 = pullVertex(base + (gl_VertexID + 2) % 3, batch.attributes);
    mat4 transform = transforms[batch.firstTransform];
    vec4 worldPos = transform * vec4(v0.position, 1);
    vec4 viewPos = constants.view * worldPos;
    gl_Position = constants.proj * viewPos;
    outUV = v0.texcoord;
    // output.normal   = normalize(cross(
    //     v2.position - v1.position,
    //     v0.position - v1.position));
}
