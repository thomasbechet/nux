#version 450

struct Batch_std430
{
    uint mode;
    uint first;
    uint count;
    uint textureWidth;
    uint textureHeight;
    vec4 color;
};

layout(binding = 2, std430) readonly buffer StructuredBuffer
{
    Batch_std430 _m0[];
} batches;

struct EntryPointParams_std430
{
    uint batchIndex;
};

uniform EntryPointParams_std430 entryPointParams;

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 0) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    do
    {
        if (batches._m0[entryPointParams.batchIndex].mode == 1u)
        {
            entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler0, input_uv);
            break;
        }
        entryPointParam_fragmentMain = batches._m0[entryPointParams.batchIndex].color;
        break;
    } while(false);
}

