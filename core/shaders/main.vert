#version 450

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 1, std430) readonly buffer StructuredBuffer
{
    float _m0[];
} storage;

layout(binding = 2, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    _MatrixStorage_float4x4_ColMajorstd140 model;
    vec2 canvas_size;
    vec2 screen_size;
    float time;
} constants;

layout(location = 0) out vec3 entryPointParam_vertexMain_normal;
layout(location = 1) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _17 = uint(gl_VertexID);
    uint base = (_17 / 3u) * 3u;
    uint _216 = _17 * 5u;
    vec3 _225 = vec3(storage._m0[_216], storage._m0[_216 + 1u], storage._m0[_216 + 2u]);
    uint _240 = (base + ((_17 + 1u) % 3u)) * 5u;
    vec3 _249 = vec3(storage._m0[_240], storage._m0[_240 + 1u], storage._m0[_240 + 2u]);
    uint _264 = (base + ((_17 + 2u) % 3u)) * 5u;
    gl_Position = ((vec4(_225, 1.0) * mat4(vec4(constants.model.data[0].x, constants.model.data[1].x, constants.model.data[2].x, constants.model.data[3].x), vec4(constants.model.data[0].y, constants.model.data[1].y, constants.model.data[2].y, constants.model.data[3].y), vec4(constants.model.data[0].z, constants.model.data[1].z, constants.model.data[2].z, constants.model.data[3].z), vec4(constants.model.data[0].w, constants.model.data[1].w, constants.model.data[2].w, constants.model.data[3].w))) * mat4(vec4(constants.view.data[0].x, constants.view.data[1].x, constants.view.data[2].x, constants.view.data[3].x), vec4(constants.view.data[0].y, constants.view.data[1].y, constants.view.data[2].y, constants.view.data[3].y), vec4(constants.view.data[0].z, constants.view.data[1].z, constants.view.data[2].z, constants.view.data[3].z), vec4(constants.view.data[0].w, constants.view.data[1].w, constants.view.data[2].w, constants.view.data[3].w))) * mat4(vec4(constants.proj.data[0].x, constants.proj.data[1].x, constants.proj.data[2].x, constants.proj.data[3].x), vec4(constants.proj.data[0].y, constants.proj.data[1].y, constants.proj.data[2].y, constants.proj.data[3].y), vec4(constants.proj.data[0].z, constants.proj.data[1].z, constants.proj.data[2].z, constants.proj.data[3].z), vec4(constants.proj.data[0].w, constants.proj.data[1].w, constants.proj.data[2].w, constants.proj.data[3].w));
    entryPointParam_vertexMain_normal = normalize(cross(vec3(storage._m0[_264], storage._m0[_264 + 1u], storage._m0[_264 + 2u]) - _249, _225 - _249));
    entryPointParam_vertexMain_uv = vec2(storage._m0[_216 + 3u], storage._m0[_216 + 4u]);
}

