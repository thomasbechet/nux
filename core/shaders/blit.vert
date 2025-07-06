#version 450

layout(location = 0) out vec2 entryPointParam_vertexMain_uv;

void main()
{
    uint _16 = uint(gl_VertexID);
    float x = float((_16 & 1u) << uint(2));
    float y = float((_16 & 2u) << uint(1));
    gl_Position = vec4(x - 1.0, y - 1.0, 0.0, 1.0);
    entryPointParam_vertexMain_uv = vec2(x, y) * 0.5;
}

