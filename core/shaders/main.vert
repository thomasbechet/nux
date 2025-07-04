#version 450

struct _MatrixStorage_float4x4_ColMajorstd430
{
    vec4 data[4];
};

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 3, std430) readonly buffer StructuredBuffer
{
    float _m0[];
} vertices;

layout(binding = 4, std430) readonly buffer transforms
{
    _MatrixStorage_float4x4_ColMajorstd430 _m0[];
} transforms_1;

layout(binding = 2, std140) uniform Constants_std140
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
    uint hasTexture;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec3 entryPointParam_vertexMain_normal;
layout(location = 1) out vec2 entryPointParam_vertexMain_uv;
layout(location = 2) out uint entryPointParam_vertexMain_hasTexture;

void main()
{
    uint _17 = uint(gl_VertexID);
    uint base = entryPointParams.vertexFirst + ((_17 / 3u) * 3u);
    uint _245 = (base + (_17 % 3u)) * 5u;
    vec3 _254 = vec3(vertices._m0[_245], vertices._m0[_245 + 1u], vertices._m0[_245 + 2u]);
    uint _269 = (base + ((_17 + 1u) % 3u)) * 5u;
    vec3 _278 = vec3(vertices._m0[_269], vertices._m0[_269 + 1u], vertices._m0[_269 + 2u]);
    uint _293 = (base + ((_17 + 2u) % 3u)) * 5u;
    gl_Position = ((vec4(_254, 1.0) * mat4(vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].x, transforms_1._m0[entryPointParams.transformIndex].data[1].x, transforms_1._m0[entryPointParams.transformIndex].data[2].x, transforms_1._m0[entryPointParams.transformIndex].data[3].x), vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].y, transforms_1._m0[entryPointParams.transformIndex].data[1].y, transforms_1._m0[entryPointParams.transformIndex].data[2].y, transforms_1._m0[entryPointParams.transformIndex].data[3].y), vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].z, transforms_1._m0[entryPointParams.transformIndex].data[1].z, transforms_1._m0[entryPointParams.transformIndex].data[2].z, transforms_1._m0[entryPointParams.transformIndex].data[3].z), vec4(transforms_1._m0[entryPointParams.transformIndex].data[0].w, transforms_1._m0[entryPointParams.transformIndex].data[1].w, transforms_1._m0[entryPointParams.transformIndex].data[2].w, transforms_1._m0[entryPointParams.transformIndex].data[3].w))) * mat4(vec4(constants.view.data[0].x, constants.view.data[1].x, constants.view.data[2].x, constants.view.data[3].x), vec4(constants.view.data[0].y, constants.view.data[1].y, constants.view.data[2].y, constants.view.data[3].y), vec4(constants.view.data[0].z, constants.view.data[1].z, constants.view.data[2].z, constants.view.data[3].z), vec4(constants.view.data[0].w, constants.view.data[1].w, constants.view.data[2].w, constants.view.data[3].w))) * mat4(vec4(constants.proj.data[0].x, constants.proj.data[1].x, constants.proj.data[2].x, constants.proj.data[3].x), vec4(constants.proj.data[0].y, constants.proj.data[1].y, constants.proj.data[2].y, constants.proj.data[3].y), vec4(constants.proj.data[0].z, constants.proj.data[1].z, constants.proj.data[2].z, constants.proj.data[3].z), vec4(constants.proj.data[0].w, constants.proj.data[1].w, constants.proj.data[2].w, constants.proj.data[3].w));
    entryPointParam_vertexMain_normal = normalize(cross(vec3(vertices._m0[_293], vertices._m0[_293 + 1u], vertices._m0[_293 + 2u]) - _278, _254 - _278));
    entryPointParam_vertexMain_uv = vec2(vertices._m0[_245 + 3u], vertices._m0[_245 + 4u]);
    entryPointParam_vertexMain_hasTexture = entryPointParams.hasTexture;
}

