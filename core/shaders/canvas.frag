#version 450

struct UniformData_std430
{
    uint mode;
    uint firstQuad;
    uint atlasWidth;
    uint atlasHeight;
};

struct EntryPointParams_std430
{
    UniformData_std430 data;
};

uniform EntryPointParams_std430 entryPointParams;

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    do
    {
        if (entryPointParams.data.mode == 1u)
        {
            entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler0, input_uv);
            break;
        }
        entryPointParam_fragmentMain = vec4(1.0);
        break;
    } while(false);
}

