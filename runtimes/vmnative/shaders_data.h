#ifndef VMN_SHADERS_DATA_H
#define VMN_SHADERS_DATA_H
#include <nulib.h>
static const nu_sv_t shader_unlit_frag = NU_SV(
    "#version 330 core\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform sampler2D texture0;\n"
    "\n"
    "in VS_OUT {\n"
    "    vec2 uv;\n"
    "} fs_in;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // frag_color = texture(texture0, fs_in.uv);\n"
    "    frag_color = vec4(1, 0, 0, 1);\n"
    "}\n");
static const nu_sv_t shader_unlit_vert = NU_SV(
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec3 in_position;\n"
    "layout(location = 1) in vec2 in_uv;\n"
    "\n"
    "uniform uvec2 viewport_size;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view_projection;\n"
    "uniform mat3 uv_transform;\n"
    "uniform sampler2D texture0;\n"
    "\n"
    "out VS_OUT {\n"
    "    vec2 uv;\n"
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
    "    // vec4 position = view_projection * model * vec4(in_position, 1);\n"
    "    // // gl_Position = snap_vertex(position);\n"
    "    // gl_Position = position;\n"
    "    //\n"
    "    // vs_out.uv = (uv_transform * vec3(in_uv, 1)).xy;\n"
    "\n"
    "    gl_Position = vec4(in_position, 1);\n"
    "}\n");
#endif
