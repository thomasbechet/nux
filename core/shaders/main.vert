#version 450

struct _MatrixStorage_float4x4_ColMajorstd430
{
    vec4 data[4];
};

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 1, std430) readonly buffer StructuredBuffer
{
    float _m0[];
} vertices;

layout(binding = 2, std430) readonly buffer transforms
{
    _MatrixStorage_float4x4_ColMajorstd430 _m0[];
} transforms_1;

layout(binding = 0, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    uvec2 canvas_size;
    uvec2 screen_size;
    float time;
} constants;

struct EntryPointParams_std430
{
    uint vertexFirst;
    uint transformIndex;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec3 entryPointParam_vertexMain_normal;
layout(location = 1) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _17 = uint(gl_VertexID);
    uint base = entryPointParams.vertexFirst + ((_17 / 3u) * 3u);
    uint _234 = (base + (_17 % 3u)) * 5u;
    vec3 _243 = vec3(vertices._m0[_234], vertices._m0[_234 + 1u], vertices._m0[_234 + 2u]);
    uint _258 = (base + ((_17 + 1u) % 3u)) * 5u;
    vec3 _267 = vec3(vertices._m0[_258], vertices._m0[_258 + 1u], vertices._m0[_258 + 2u]);
    uint _282 = (base + ((_17 + 2u) % 3u)) * 5u;
    gl_Position = ((vec4(_243, 1.0) * mat4(vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].x, transforms_1._m0[entryPointParams.transformIndex].data[1].x, transforms_1._m0[entryPointParams.transformIndex].data[2].x, transforms_1._m0[entryPointParams.transformIndex].data[3].x), vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].y, transforms_1._m0[entryPointParams.transformIndex].data[1].y, transforms_1._m0[entryPointParams.transformIndex].data[2].y, transforms_1._m0[entryPointParams.transformIndex].data[3].y), vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].z, transforms_1._m0[entryPointParams.transformIndex].data[1].z, transforms_1._m0[entryPointParams.transformIndex].data[2].z, transforms_1._m0[entryPointParams.transformIndex].data[3].z), vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].w, transforms_1._m0[entryPointParams.transformIndex].data[1].w, transforms_1._m0[entryPointParams.transformIndex].data[2].w, transforms_1._m0[entryPointParams.transformIndex].data[3].w))) * mat4(vec4(constants.view.data[0].x, constants.view.data[1].x, constants.view.data[2].x, constants.view.data[3].x), vec4(constants.view.data[0].y, constants.view.data[1].y, constants.view.data[2].y, constants.view.data[3].y), vec4(constants.view.data[0].z, constants.view.data[1].z, constants.view.data[2].z, constants.view.data[3].z), vec4(constants.view.data[0].w, constants.view.data[1].w, constants.view.data[2].w, constants.view.data[3].w))) * mat4(vec4(constants.proj.data[0].x, constants.proj.data[1].x, constants.proj.data[2].x, constants.proj.data[3].x), vec4(constants.proj.data[0].y, constants.proj.data[1].y, constants.proj.data[2].y, constants.proj.data[3].y), vec4(constants.proj.data[0].z, constants.proj.data[1].z, constants.proj.data[2].z, constants.proj.data[3].z), vec4(constants.proj.data[0].w, constants.proj.data[1].w, constants.proj.data[2].w, constants.proj.data[3].w));
    entryPointParam_vertexMain_normal = normalize(cross(vec3(vertices._m0[_282], vertices._m0[_282 + 1u], vertices._m0[_282 + 2u]) - _267, _243 - _267));
    entryPointParam_vertexMain_uv = vec2(vertices._m0[_234 + 3u], vertices._m0[_234 + 4u]);
}

