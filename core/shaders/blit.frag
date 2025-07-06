#version 450

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 4, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    uvec2 canvas_size;
    uvec2 screen_size;
    float time;
} constants;

uniform usampler2D SPIRV_Cross_Combinedcanvassampler0;
uniform sampler2D SPIRV_Cross_Combinedcolormapsampler1;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    vec2 _24 = vec2(constants.canvas_size);
    vec2 _143 = vec2(input_uv.x, 1.0 - input_uv.y) * _24;
    vec2 _146 = floor(_143 + vec2(0.5));
    vec2 _161 = (((_146 + clamp((_143 - _146) / fwidth(_143), vec2(-0.5), vec2(0.5))) / _24) * _24) - vec2(0.5);
    vec2 _162 = floor(_161);
    vec2 _163 = fract(_161);
    float _176 = _163.x;
    float _179 = _163.y;
    entryPointParam_fragmentMain = (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_162 + vec2(1.5)) / _24).x) * 0.0039215688593685626983642578125, 0.0)) * _176) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_162 + vec2(0.5, 1.5)) / _24).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _176))) * _179) + (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_162 + vec2(1.5, 0.5)) / _24).x) * 0.0039215688593685626983642578125, 0.0)) * _176) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_162 + vec2(0.5)) / _24).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _176))) * (1.0 - _179));
}

