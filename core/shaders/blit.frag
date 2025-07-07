#version 450

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 2, std140) uniform Constants_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    uvec2 canvas_size;
    uvec2 screen_size;
    float time;
} constants;

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    vec2 _24 = vec2(constants.canvas_size);
    vec2 _126 = input_uv * _24;
    vec2 _129 = floor(_126 + vec2(0.5));
    vec2 _144 = (((_129 + clamp((_126 - _129) / fwidth(_126), vec2(-0.5), vec2(0.5))) / _24) * _24) - vec2(0.5);
    vec2 _145 = floor(_144);
    vec2 _146 = fract(_144);
    float _159 = _146.x;
    float _162 = _146.y;
    entryPointParam_fragmentMain = (((texture(SPIRV_Cross_Combinedtexture0sampler0, (_145 + vec2(1.5)) / _24) * _159) + (texture(SPIRV_Cross_Combinedtexture0sampler0, (_145 + vec2(0.5, 1.5)) / _24) * (1.0 - _159))) * _162) + (((texture(SPIRV_Cross_Combinedtexture0sampler0, (_145 + vec2(1.5, 0.5)) / _24) * _159) + (texture(SPIRV_Cross_Combinedtexture0sampler0, (_145 + vec2(0.5)) / _24) * (1.0 - _159))) * (1.0 - _162));
}

