#version 450

struct _MatrixStorage_float4x4_ColMajorstd140
{
    vec4 data[4];
};

layout(binding = 0, std430) readonly buffer StructuredBuffer
{
    float _m0[];
} storage;

layout(binding = 1, std140) uniform Uniform_std140
{
    _MatrixStorage_float4x4_ColMajorstd140 view;
    _MatrixStorage_float4x4_ColMajorstd140 proj;
    _MatrixStorage_float4x4_ColMajorstd140 model;
} _uniform;

layout(location = 0) out vec3 entryPointParam_vertexMain_normal;
layout(location = 1) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _197 = uint(gl_VertexID) * 5u;
    vec4 _173 = ((vec4(vec3(storage._m0[_197], storage._m0[_197 + 1u], storage._m0[_197 + 2u]), 1.0) * mat4(vec4(_uniform.model.data[0].x, _uniform.model.data[1].x, _uniform.model.data[2].x, _uniform.model.data[3].x), vec4(_uniform.model.data[0].y, _uniform.model.data[1].y, _uniform.model.data[2].y, _uniform.model.data[3].y), vec4(_uniform.model.data[0].z, _uniform.model.data[1].z, _uniform.model.data[2].z, _uniform.model.data[3].z), vec4(_uniform.model.data[0].w, _uniform.model.data[1].w, _uniform.model.data[2].w, _uniform.model.data[3].w))) * mat4(vec4(_uniform.view.data[0].x, _uniform.view.data[1].x, _uniform.view.data[2].x, _uniform.view.data[3].x), vec4(_uniform.view.data[0].y, _uniform.view.data[1].y, _uniform.view.data[2].y, _uniform.view.data[3].y), vec4(_uniform.view.data[0].z, _uniform.view.data[1].z, _uniform.view.data[2].z, _uniform.view.data[3].z), vec4(_uniform.view.data[0].w, _uniform.view.data[1].w, _uniform.view.data[2].w, _uniform.view.data[3].w))) * mat4(vec4(_uniform.proj.data[0].x, _uniform.proj.data[1].x, _uniform.proj.data[2].x, _uniform.proj.data[3].x), vec4(_uniform.proj.data[0].y, _uniform.proj.data[1].y, _uniform.proj.data[2].y, _uniform.proj.data[3].y), vec4(_uniform.proj.data[0].z, _uniform.proj.data[1].z, _uniform.proj.data[2].z, _uniform.proj.data[3].z), vec4(_uniform.proj.data[0].w, _uniform.proj.data[1].w, _uniform.proj.data[2].w, _uniform.proj.data[3].w));
    gl_Position = _173;
    entryPointParam_vertexMain_normal = vec3(0.0);
    entryPointParam_vertexMain_uv = vec2(storage._m0[_197 + 3u], storage._m0[_197 + 4u]);
}

