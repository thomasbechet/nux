#version 450

struct EntryPointParams_std430
{
    uint textureWidth;
    uint textureHeight;
};

uniform EntryPointParams_std430 entryPointParams;

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    vec2 textureSize = vec2(float(entryPointParams.textureWidth), float(entryPointParams.textureHeight));
    vec2 _127 = input_uv * textureSize;
    vec2 _130 = floor(_127 + vec2(0.5));
    vec2 _145 = (((_130 + clamp((_127 - _130) / fwidth(_127), vec2(-0.5), vec2(0.5))) / textureSize) * textureSize) - vec2(0.5);
    vec2 _146 = floor(_145);
    vec2 _147 = fract(_145);
    float _160 = _147.x;
    float _163 = _147.y;
    entryPointParam_fragmentMain = (((texture(SPIRV_Cross_Combinedtexture0sampler0, (_146 + vec2(1.5)) / textureSize) * _160) + (texture(SPIRV_Cross_Combinedtexture0sampler0, (_146 + vec2(0.5, 1.5)) / textureSize) * (1.0 - _160))) * _163) + (((texture(SPIRV_Cross_Combinedtexture0sampler0, (_146 + vec2(1.5, 0.5)) / textureSize) * _160) + (texture(SPIRV_Cross_Combinedtexture0sampler0, (_146 + vec2(0.5)) / textureSize) * (1.0 - _160))) * (1.0 - _163));
}

