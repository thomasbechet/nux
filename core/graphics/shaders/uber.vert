#version 450

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
};

struct Vertex
{
    vec3 position;
    vec2 texcoord;
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

Vertex pullVertex(uint idx)
{
    Vertex vertex;
    vertex.position = vec3(
            vertices[idx * 5 + 0],
            vertices[idx * 5 + 1],
            vertices[idx * 5 + 2]
        );
    vertex.texcoord = vec2(
            vertices[idx * 5 + 3],
            vertices[idx * 5 + 4]
        );
    return vertex;
}

void main()
{
    Batch batch = batches[batchIndex];
    uint base = batch.firstVertex + (gl_VertexID / 3) * 3;
    Vertex v0 = pullVertex(base + (gl_VertexID + 0) % 3);
    Vertex v1 = pullVertex(base + (gl_VertexID + 1) % 3);
    Vertex v2 = pullVertex(base + (gl_VertexID + 2) % 3);
    mat4 transform = transforms[batch.firstTransform];
    vec4 worldPos = transform * vec4(v0.position, 1);
    vec4 viewPos = constants.view * worldPos;
    gl_Position = constants.proj * viewPos;
    outUV = v0.texcoord;
    // output.normal   = normalize(cross(
    //     v2.position - v1.position,
    //     v0.position - v1.position));
}
