#version 450

struct Batch_std430
{
    uint firstVertex;
    uint firstTransform;
    uint hasTexture;
};

struct _MatrixStorage_float4x4_ColMajorstd430
{
    vec4 data[4];
};

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 4, std430) readonly buffer StructuredBuffer
{
    Batch_std430 _m0[];
} batches;

layout(binding = 3, std430) readonly buffer vertices
{
    float _m0[];
} vertices_1;

layout(binding = 5, std430) readonly buffer transforms
{
    _MatrixStorage_float4x4_ColMajorstd430 _m0[];
} transforms_1;

layout(binding = 2, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    uvec2 screen_size;
    float time;
} constants;

struct EntryPointParams_std430
{
    uint batchIndex;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec3 entryPointParam_vertexMain_normal;
layout(location = 1) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _17 = uint(gl_VertexID);
    uint _227 = ((batches._m0[entryPointParams.batchIndex].firstVertex + ((_17 / 3u) * 3u)) + (_17 % 3u)) * 5u;
    vec4 _202 = ((vec4(vec3(vertices_1._m0[_227], vertices_1._m0[_227 + 1u], vertices_1._m0[_227 + 2u]), 1.0) * mat4(vec4(transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[0].x, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[1].x, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[2].x, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[3].x), vec4(transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[0].y, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[1].y, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[2].y, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[3].y), vec4(transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[0].z, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[1].z, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[2].z, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[3].z), vec4(transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[0].w, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[1].w, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[2].w, transforms_1._m0[batches._m0[entryPointParams.batchIndex].firstTransform].data[3].w))) * mat4(vec4(constants.view.data[0].x, constants.view.data[1].x, constants.view.data[2].x, constants.view.data[3].x), vec4(constants.view.data[0].y, constants.view.data[1].y, constants.view.data[2].y, constants.view.data[3].y), vec4(constants.view.data[0].z, constants.view.data[1].z, constants.view.data[2].z, constants.view.data[3].z), vec4(constants.view.data[0].w, constants.view.data[1].w, constants.view.data[2].w, constants.view.data[3].w))) * mat4(vec4(constants.proj.data[0].x, constants.proj.data[1].x, constants.proj.data[2].x, constants.proj.data[3].x), vec4(constants.proj.data[0].y, constants.proj.data[1].y, constants.proj.data[2].y, constants.proj.data[3].y), vec4(constants.proj.data[0].z, constants.proj.data[1].z, constants.proj.data[2].z, constants.proj.data[3].z), vec4(constants.proj.data[0].w, constants.proj.data[1].w, constants.proj.data[2].w, constants.proj.data[3].w));
    gl_Position = _202;
    entryPointParam_vertexMain_normal = vec3(0.0);
    entryPointParam_vertexMain_uv = vec2(vertices_1._m0[_227 + 3u], vertices_1._m0[_227 + 4u]);
}

