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

layout(location = 0) in vec3 input_normal;
layout(location = 1) in vec2 input_uv;
layout(location = 2) flat in uint input_hasTexture;
layout(location = 0) out vec4 entryPointParam_fragmentMain;

void main()
{
    do
    {
        float _40 = dot(input_normal, normalize(vec3(cos(constants.time), 1.0, sin(constants.time))));
        if (input_hasTexture != 0u)
        {
            entryPointParam_fragmentMain = texture(SPIRV_Cross_Combinedtexture0sampler0, input_uv) * max(_40, 0.5);
            break;
        }
        else
        {
            entryPointParam_fragmentMain = vec4(input_uv, 0.0, 1.0) * max(_40, 0.5);
            break;
        }
        break;
    } while(false);
}

