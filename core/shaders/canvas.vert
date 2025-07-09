#version 450

struct Batch_std430
{
    uint mode;
    uint first;
    uint count;
    uint textureWidth;
    uint textureHeight;
    vec4 color;
};

const vec2 _78[6] = vec2[](vec2(0.0), vec2(0.0, 1.0), vec2(1.0), vec2(1.0), vec2(1.0, 0.0), vec2(0.0));

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 2, std430) readonly buffer StructuredBuffer
{
    Batch_std430 _m0[];
} batches;

layout(binding = 1, std430) readonly buffer quads
{
    uint _m0[];
} quads_1;

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
    uint batchIndex;
};

uniform EntryPointParams_std430 entryPointParams;

layout(location = 0) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _22 = uint(gl_VertexID);
    uint _44 = (batches._m0[entryPointParams.batchIndex].first + (_22 / 6u)) * 3u;
    uint _52 = _44 + 1u;
    uint _56 = _44 + 2u;
    vec2 _88 = vec2(float(quads_1._m0[_56] & 65535u), float(quads_1._m0[_56] >> 16u)) * _78[_22 % 6u];
    vec2 _102 = (vec2(float(quads_1._m0[_44] & 65535u), float(quads_1._m0[_44] >> 16u)) + _88) / vec2(constants.screen_size);
    _102.y = 1.0 - _102.y;
    gl_Position = vec4((_102 * 2.0) - vec2(1.0), 0.0, 1.0);
    entryPointParam_vertexMain_uv = floor(vec2(float(quads_1._m0[_52] & 65535u), float(quads_1._m0[_52] >> 16u)) + _88) / vec2(float(batches._m0[entryPointParams.batchIndex].textureWidth), float(batches._m0[entryPointParams.batchIndex].textureHeight));
}

