#ifndef SHADERS_DATA_H
#define SHADERS_DATA_H
#include <nulib/nulib.h>
static const nu_sv_t shader_canvas_blit_frag = NU_SV(
"#version 330\n"
"\n"
"in vec2 uv;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D texture0;\n"
"\n"
"void main()\n"
"{\n"
"    color = texture(texture0, uv);\n"
"}\n"
);
static const nu_sv_t shader_canvas_blit_vert = NU_SV(
"#version 330\n"
"\n"
"layout(location = 0) in uint pos;\n"
"layout(location = 1) in uint tex;\n"
"layout(location = 2) in uint size;\n"
"\n"
"uniform sampler2D texture0;\n"
"uniform uvec2 viewport_size;\n"
"\n"
"out vec2 uv;\n"
"\n"
"const vec2 offsets[6] = vec2[](\n"
"    vec2(0, 0),\n"
"    vec2(0, 1),\n"
"    vec2(1, 1),\n"
"    vec2(1, 1),\n"
"    vec2(1, 0),\n"
"    vec2(0, 0)\n"
");\n"
"\n"
"void main()\n"
"{\n"
"    // Extract vertex data\n"
"    vec2 vertex_pos = vec2(float(pos & 0xffffu), float(pos >> 16u)); \n"
"    vec2 vertex_tex = vec2(float(tex & 0xffffu), float(tex >> 16u));\n"
"    vec2 vertex_size = vec2(float(size & 0xffffu), float(size >> 16u));\n"
"\n"
"    // Compute vertex offset based on the vertex index\n"
"    vec2 vertex_offset = offsets[gl_VertexID];\n"
"\n"
"    // Apply offset and normalize\n"
"    vec2 position = (vertex_pos + vertex_size * vertex_offset) / vec2(viewport_size); \n"
"    position.y = 1 - position.y;\n"
"    gl_Position = vec4(position * 2 - 1, 0, 1);\n"
"\n"
"    // Set output\n"
"    uv = floor(vertex_tex + vertex_size * vertex_offset) / vec2(textureSize(texture0, 0));\n"
"}\n"
);
static const nu_sv_t shader_screen_blit_frag = NU_SV(
"#version 330 core\n"
"\n"
"out vec4 out_color;\n"
"in vec2 uv;\n"
"\n"
"uniform sampler2D t_surface;\n"
"\n"
"vec2 uv_filtering(in vec2 uv, in vec2 texture_size)\n"
"{\n"
"    vec2 pixel = uv * texture_size;\n"
"    vec2 seam = floor(pixel + 0.5);\n"
"    vec2 dudv = fwidth(pixel);\n"
"    vec2 rel = (pixel - seam) / dudv;\n"
"    vec2 mid_pix = vec2(0.5);\n"
"    pixel = seam + clamp(rel, -mid_pix, mid_pix);\n"
"    return pixel / texture_size;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec2 filtered_uv = uv_filtering(uv, textureSize(t_surface, 0));\n"
"    out_color = texture(t_surface, filtered_uv);\n"
"}\n"
);
static const nu_sv_t shader_screen_blit_vert = NU_SV(
"#version 330 core\n"
"\n"
"out vec2 uv;\n"
"\n"
"void main()\n"
"{\n"
"    float x = float((gl_VertexID & 1) << 2);\n"
"    float y = float((gl_VertexID & 2) << 1);\n"
"\n"
"    uv.x = x * 0.5;\n"
"    uv.y = y * 0.5;\n"
"\n"
"    gl_Position = vec4(x - 1.0, y - 1.0, 0.0, 1.0);\n"
"}\n"
);
static const nu_sv_t shader_unlit_frag = NU_SV(
"#version 330 core\n"
"\n"
"out vec4 frag_color;\n"
"\n"
"uniform sampler2D texture0;\n"
"\n"
"layout(std140) uniform UBO {\n"
"    mat4 view;\n"
"    mat4 projection;\n"
"    vec4 color;\n"
"    vec4 fog_color;\n"
"    uvec2 viewport_size;\n"
"    float fog_density;\n"
"    float fog_near;\n"
"    float fog_far;\n"
"};\n"
"\n"
"in VS_OUT {\n"
"    vec2 uv;\n"
"    vec3 color;\n"
"    float dist_cam;\n"
"} fs_in;\n"
"\n"
"void main()\n"
"{\n"
"    vec4 color = texture(texture0, fs_in.uv) * vec4(fs_in.color, 1);\n"
"    float fog = clamp((fs_in.dist_cam - fog_near) / (fog_far - fog_near), 0, 1) * fog_density;\n"
"    frag_color = mix(color, fog_color, fog);\n"
"}\n"
);
static const nu_sv_t shader_unlit_vert = NU_SV(
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 in_position;\n"
"layout(location = 1) in vec2 in_uv;\n"
"layout(location = 2) in vec3 in_color;\n"
"\n"
"layout(std140) uniform UBO {\n"
"    mat4 view;\n"
"    mat4 projection;\n"
"    vec4 color;\n"
"    vec4 fog_color;\n"
"    uvec2 viewport_size;\n"
"    float fog_density;\n"
"    float fog_near;\n"
"    float fog_far;\n"
"};\n"
"\n"
"uniform mat4 model;\n"
"\n"
"out VS_OUT {\n"
"    vec2 uv;\n"
"    vec3 color;\n"
"    float dist_cam;\n"
"} vs_out;\n"
"\n"
"vec4 snap_vertex(in vec4 position)\n"
"{\n"
"    vec2 grid = vec2(viewport_size);\n"
"    position.xy = position.xy / position.w;\n"
"    position.xy = (floor(grid * position.xy) + 0.5) / grid;\n"
"    position.xy *= position.w;\n"
"    return position;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec4 position = view * model * vec4(in_position, 1);\n"
"    vs_out.dist_cam = length(position);\n"
"    position = projection * position;\n"
"    gl_Position = snap_vertex(position);\n"
"\n"
"    vs_out.uv = in_uv;\n"
"    vs_out.color = in_color * color.xyz;\n"
"}\n"
);
#endif
