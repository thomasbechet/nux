#version 450

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 4, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    _MatrixStorage_float4x4_ColMajorstd140 model;
    vec2 canvas_size;
    vec2 screen_size;
    float time;
} constants;

uniform usampler2D SPIRV_Cross_Combinedcanvassampler0;
uniform sampler2D SPIRV_Cross_Combinedcolormapsampler1;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    vec2 _141 = vec2(input_uv.x, 1.0 - input_uv.y) * constants.canvas_size;
    vec2 _144 = floor(_141 + vec2(0.5));
    vec2 _159 = (((_144 + clamp((_141 - _144) / fwidth(_141), vec2(-0.5), vec2(0.5))) / constants.canvas_size) * constants.canvas_size) - vec2(0.5);
    vec2 _160 = floor(_159);
    vec2 _161 = fract(_159);
    float _174 = _161.x;
    float _177 = _161.y;
    entryPointParam_fragmentMain = (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_160 + vec2(1.5)) / constants.canvas_size).x) * 0.0039215688593685626983642578125, 0.0)) * _174) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_160 + vec2(0.5, 1.5)) / constants.canvas_size).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _174))) * _177) + (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_160 + vec2(1.5, 0.5)) / constants.canvas_size).x) * 0.0039215688593685626983642578125, 0.0)) * _174) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_160 + vec2(0.5)) / constants.canvas_size).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _174))) * (1.0 - _177));
}

