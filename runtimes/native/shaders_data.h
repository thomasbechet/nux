#ifndef SHADERS_DATA_H
#define SHADERS_DATA_H
#include <nulib/nulib.h>
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
    "}\n");
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
    "}\n");
static const nu_sv_t shader_unlit_frag = NU_SV(
    "#version 330 core\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform sampler2D texture0;\n"
    "\n"
    "in VS_OUT {\n"
    "    vec2 uv;\n"
    "    vec3 color;\n"
    "} fs_in;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    frag_color = texture(texture0, fs_in.uv) * vec4(fs_in.color, 1);\n"
    "}\n");
static const nu_sv_t shader_unlit_vert = NU_SV(
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec3 in_position;\n"
    "layout(location = 1) in vec2 in_uv;\n"
    "layout(location = 2) in vec3 in_color;\n"
    "\n"
    "uniform uvec2 viewport_size;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view_projection;\n"
    "\n"
    "out VS_OUT {\n"
    "    vec2 uv;\n"
    "    vec3 color;\n"
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
    "    vec4 position = view_projection * model * vec4(in_position, 1);\n"
    "    gl_Position = snap_vertex(position);\n"
    "    // gl_Position = position;\n"
    "\n"
    "    vs_out.uv = in_uv;\n"
    "    vs_out.color = in_color;\n"
    "}\n");
#endif
