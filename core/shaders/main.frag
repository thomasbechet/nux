#version 450

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 1) in vec2 input_uv;
layout(location = 2) flat in uint input_hasTexture;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    do
    {
        if (input_hasTexture != 0u)
        {
            entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler0, input_uv) * 1.0;
            break;
        }
        else
        {
            entryPointParam_fragmentMain = vec4(input_uv, 0.0, 1.0) * 1.0;
            break;
        }
        break;
    } while(false);
}

