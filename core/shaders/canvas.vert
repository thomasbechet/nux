#version 450

const vec2 _57[6] = vec2[](vec2(0.0), vec2(0.0, 1.0), vec2(1.0), vec2(1.0), vec2(1.0, 0.0), vec2(0.0));

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 1, std430) readonly buffer StructuredBuffer
{
    uint _m0[];
} quads;

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
    uint firstQuad;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _22 = uint(gl_VertexID);
    uint index = entryPointParams.firstQuad + (_22 / 6u);
    uint _40 = index + 2u;
    vec2 _81 = (vec2(float(quads._m0[index] & 65535u), float(quads._m0[index] >> 16u)) + (vec2(float(quads._m0[_40] & 65535u), float(quads._m0[_40] >> 16u)) * _57[_22 % 6u])) / vec2(constants.screen_size);
    _81.y = 1.0 - _81.y;
    gl_Position = vec4((_81 * 2.0) - vec2(1.0), 0.0, 1.0);
    entryPointParam_vertexMain_uv = vec2(0.0);
}

