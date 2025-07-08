#version 450

struct UniformData_std430
{
    uint mode;
    uint firstQuad;
    uint atlasWidth;
    uint atlasHeight;
};

const vec2 _72[6] = vec2[](vec2(0.0), vec2(0.0, 1.0), vec2(1.0), vec2(1.0), vec2(1.0, 0.0), vec2(0.0));

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
    UniformData_std430 data;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _22 = uint(gl_VertexID);
    uint _37 = (entryPointParams.data.firstQuad + (_22 / 6u)) * 3u;
    uint _46 = _37 + 1u;
    uint _50 = _37 + 2u;
    vec2 _82 = vec2(float(quads._m0[_50] & 65535u), float(quads._m0[_50] >> 16u)) * _72[_22 % 6u];
    vec2 _96 = (vec2(float(quads._m0[_37] & 65535u), float(quads._m0[_37] >> 16u)) + _82) / vec2(constants.screen_size);
    _96.y = 1.0 - _96.y;
    gl_Position = vec4((_96 * 2.0) - vec2(1.0), 0.0, 1.0);
    entryPointParam_vertexMain_uv = floor(vec2(float(quads._m0[_46] & 65535u), float(quads._m0[_46] >> 16u)) + _82) / vec2(float(entryPointParams.data.atlasWidth), float(entryPointParams.data.atlasHeight));
}

