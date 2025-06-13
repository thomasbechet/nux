#version 450

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 2, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    _MatrixStorage_float4x4_ColMajorstd140 model;
    uvec2 canvas_size;
    uvec2 screen_size;
    float time;
    vec3 _pad;
} constants;

layout(location = 0) in vec3 input_normal;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    entryPointParam_fragmentMain = vec4(1.0, 0.0, 0.0, 1.0) * max(dot(input_normal, normalize(vec3(cos(constants.time), 1.0, sin(constants.time)))), 0.5);
}

