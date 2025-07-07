#version 450

const vec2 _68[6] = vec2[](vec2(0.0), vec2(0.0, 1.0), vec2(1.0), vec2(1.0), vec2(1.0, 0.0), vec2(0.0));

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
    uint atlasWidth;
    uint atlasHeight;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _22 = uint(gl_VertexID);
    uint _33 = (entryPointParams.firstQuad + (_22 / 6u)) * 3u;
    uint _42 = _33 + 1u;
    uint _46 = _33 + 2u;
    vec2 _78 = vec2(float(quads._m0[_46] & 65535u), float(quads._m0[_46] >> 16u)) * _68[_22 % 6u];
    vec2 _92 = (vec2(float(quads._m0[_33] & 65535u), float(quads._m0[_33] >> 16u)) + _78) / vec2(constants.screen_size);
    _92.y = 1.0 - _92.y;
    gl_Position = vec4((_92 * 2.0) - vec2(1.0), 0.0, 1.0);
    entryPointParam_vertexMain_uv = floor(vec2(float(quads._m0[_42] & 65535u), float(quads._m0[_42] >> 16u)) + _78) / vec2(float(entryPointParams.atlasWidth), float(entryPointParams.atlasHeight));
}

