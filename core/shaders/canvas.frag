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
    uvec2 canvas_size;
    uvec2 screen_size;
    float time;
    vec3 _pad;
} constants;

uniform usampler2D SPIRV_Cross_Combinedcanvassampler0;
uniform sampler2D SPIRV_Cross_Combinedcolormapsampler1;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    vec2 _25 = vec2(constants.canvas_size);
    vec2 _144 = vec2(input_uv.x, 1.0 - input_uv.y) * _25;
    vec2 _147 = floor(_144 + vec2(0.5));
    vec2 _162 = (((_147 + clamp((_144 - _147) / fwidth(_144), vec2(-0.5), vec2(0.5))) / _25) * _25) - vec2(0.5);
    vec2 _163 = floor(_162);
    vec2 _164 = fract(_162);
    float _177 = _164.x;
    float _180 = _164.y;
    entryPointParam_fragmentMain = (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_163 + vec2(1.5)) / _25).x) * 0.0039215688593685626983642578125, 0.0)) * _177) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_163 + vec2(0.5, 1.5)) / _25).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _177))) * _180) + (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_163 + vec2(1.5, 0.5)) / _25).x) * 0.0039215688593685626983642578125, 0.0)) * _177) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_163 + vec2(0.5)) / _25).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _177))) * (1.0 - _180));
}

