#version 450

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler;

layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler, vec2(0.0));
}

