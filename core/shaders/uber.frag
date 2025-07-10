#version 450

struct Batch_std430
{
    uint firstVertex;
    uint firstTransform;
    uint hasTexture;
};

layout(binding = 4, std430) readonly buffer StructuredBuffer
{
    Batch_std430 _m0[];
} batches;

struct EntryPointParams_std430
{
    uint batchIndex;
};

uniform EntryPointParams_std430 entryPointParams;

uniform sampler2D SPIRV_Cross_Combinedtexture0sampler0;

layout(location = 1) in vec2 input_uv;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    do
    {
        if (batches._m0[entryPointParams.batchIndex].hasTexture != 0u)
        {
            entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler0, input_uv);
            break;
        }
        else
        {
            entryPointParam_fragmentMain = vec4(input_uv, 0.0, 1.0);
            break;
        }
        break;
    } while(false);
}

