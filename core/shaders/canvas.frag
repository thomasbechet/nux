#version 450

uniform usampler2D SPIRV_Cross_Combinedcanvassampler0;
uniform sampler2D SPIRV_Cross_Combinedcolormapsampler1;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    vec2 _133 = vec2(input_uv.x, 1.0 - input_uv.y) * vec2(480.0, 300.0);
    vec2 _136 = floor(_133 + vec2(0.5));
    vec2 _151 = (((_136 + clamp((_133 - _136) / fwidth(_133), vec2(-0.5), vec2(0.5))) * vec2(0.00208333344198763370513916015625, 0.0033333334140479564666748046875)) * vec2(480.0, 300.0)) - vec2(0.5);
    vec2 _152 = floor(_151);
    vec2 _153 = fract(_151);
    float _166 = _153.x;
    float _169 = _153.y;
    entryPointParam_fragmentMain = (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_152 + vec2(1.5)) * vec2(0.00208333344198763370513916015625, 0.0033333334140479564666748046875)).x) * 0.0039215688593685626983642578125, 0.0)) * _166) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_152 + vec2(0.5, 1.5)) * vec2(0.00208333344198763370513916015625, 0.0033333334140479564666748046875)).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _166))) * _169) + (((texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_152 + vec2(1.5, 0.5)) * vec2(0.00208333344198763370513916015625, 0.0033333334140479564666748046875)).x) * 0.0039215688593685626983642578125, 0.0)) * _166) + (texture(SPIRV_Cross_Combinedcolormapsampler1, vec2(float(texture(SPIRV_Cross_Combinedcanvassampler0, (_152 + vec2(0.5)) * vec2(0.00208333344198763370513916015625, 0.0033333334140479564666748046875)).x) * 0.0039215688593685626983642578125, 0.0)) * (1.0 - _166))) * (1.0 - _169));
}

