#version 450

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler0, input_uv);
}

