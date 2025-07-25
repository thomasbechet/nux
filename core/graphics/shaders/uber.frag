#version 450

struct Batch
{
    uint firstVertex;
    uint firstTransform;
    uint hasTexture;
};

layout(binding = 3, std430) readonly buffer BatchBlock
{
    Batch batches[];
};

layout(location = 1) in vec2 inUV;
layout(location = 0) out vec4 outColor;

uniform sampler2D texture0;
uniform uint batchIndex;

void main()
{
    if (batches[batchIndex].hasTexture != 0)
    {
        outColor = texture(texture0, inUV);
    }
    else
    {
        outColor = vec4(inUV, 0, 1);
    }
}
