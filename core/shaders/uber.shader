#version 330 core

#ifdef SUPPORT_SSBO
layout(std430, binding = 0) readonly buffer GPUBuffer
{
    uint in_data[];
};
#else
texelFetch
#endif

uniform uint in_base;
uniform usampler2D in_texture;
uniform sampler2D in_colormap;

uint fetchWord(in int idx)
{
#ifdef SUPPORT_SSBO
    return in_data[idx];
#else

#endif
}

vec3 decodeVec3(in int idx)
{
    return vec3(
        uintBitsToFloat(in_data[idx + 0]),
        uintBitsToFloat(in_data[idx + 1]),
        uintBitsToFloat(in_data[idx + 2]));
}
vec4 decodeVec4(in int idx)
{
    return vec4(
        uintBitsToFloat(in_data[idx + 0]),
        uintBitsToFloat(in_data[idx + 1]),
        uintBitsToFloat(in_data[idx + 2]),
        uintBitsToFloat(in_data[idx + 3]));
}
mat4 decodeMat4(in int idx)
{
    return mat4(
        decodeVec4(idx + 0),
        decodeVec4(idx + 4),
        decodeVec4(idx + 8),
        decodeVec4(idx + 12));
}

#ifdef VERTEX_SHADER

out VS_OUT {
    vec2 uv;
    vec4 color;
} vs_out;

void main()
{
    vec3 pos = decodeVec3(in_base + gl_VertexID);
    gl_Position = vec4(1.0);
}

#elif FRAGMENT_SHADER

in VS_IN {
    vec2 uv;
    vec4 color;
} vs_out;

out vec4 color;

void main()
{
    color = vec4(0);
}

#endif
