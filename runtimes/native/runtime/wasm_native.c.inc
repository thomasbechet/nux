#include "core/vm.h"
#include <wasm_export.h>

static void
trace (wasm_exec_env_t env, const nu_char_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_trace(vm, text);
}
static void
inspect_i32 (wasm_exec_env_t env, const nu_char_t *name, nu_i32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_inspect_i32(vm, name, p);
}
static void
inspect_f32 (wasm_exec_env_t env, const nu_char_t *name, nu_f32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_inspect_f32(vm, name, p);
}
static nu_u32_t
console_info (wasm_exec_env_t env, sys_console_info_t info)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_console_info(vm, info);
}
static nu_f32_t
global_time (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_global_time(vm);
}
static nu_f32_t
delta_time (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_delta_time(vm);
}
static nu_status_t
create_scope (wasm_exec_env_t env, nu_u32_t id, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_scope(vm, id, size);
}
static nu_status_t
rewind_scope (wasm_exec_env_t env, nu_u32_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_rewind_scope(vm, id);
}
static void
set_active_scope (wasm_exec_env_t env, nu_u32_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_set_active_scope(vm, id);
}
static nu_status_t
create_camera (wasm_exec_env_t env, nu_u32_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_camera(vm, id);
}
static void
set_camera_view (wasm_exec_env_t env, nu_u32_t id, const nu_f32_t *m)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_set_camera_view(vm, id, m);
}
static void
set_camera_projection (wasm_exec_env_t env, nu_u32_t id, const nu_f32_t *m)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_set_camera_projection(vm, id, m);
}
static void
set_camera_lookat (wasm_exec_env_t env,
                   nu_u32_t        id,
                   const nu_f32_t *eye,
                   const nu_f32_t *center,
                   const nu_f32_t *up)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_set_camera_lookat(vm, id, eye, center, up);
}
static void
set_camera_perspective (
    wasm_exec_env_t env, nu_u32_t id, nu_f32_t fov, nu_f32_t near, nu_f32_t far)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_set_camera_perspective(vm, id, fov, near, far);
}
static nu_status_t
create_texture (wasm_exec_env_t env, nu_u32_t id, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_texture(vm, id, size);
}
static nu_status_t
update_texture (wasm_exec_env_t env,
                nu_u32_t        id,
                nu_u32_t        x,
                nu_u32_t        y,
                nu_u32_t        w,
                nu_u32_t        h,
                const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_update_texture(vm, id, x, y, w, h, p);
}
static nu_status_t
create_mesh (wasm_exec_env_t        env,
             nu_u32_t               id,
             nu_u32_t               count,
             sys_primitive_t        primitive,
             sys_vertex_attribute_t attribs)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_mesh(vm, id, count, primitive, attribs);
}
static nu_status_t
update_mesh (wasm_exec_env_t        env,
             nu_u32_t               id,
             sys_vertex_attribute_t attribs,
             nu_u32_t               first,
             nu_u32_t               count,
             const void            *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_update_mesh(vm, id, attribs, first, count, p);
}
static nu_status_t
create_model (wasm_exec_env_t env, nu_u32_t id, nu_u32_t count)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_model(vm, id, count);
}
static nu_status_t
update_model (wasm_exec_env_t env,
              nu_u32_t        id,
              nu_u32_t        node,
              nu_u32_t        mesh,
              nu_u32_t        texture,
              nu_u32_t        parent,
              const nu_f32_t *transform)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_update_model(vm, id, node, mesh, texture, parent, transform);
}
static nu_status_t
create_spritesheet (wasm_exec_env_t env,
                    nu_u32_t        id,
                    nu_u32_t        texture,
                    nu_u32_t        row,
                    nu_u32_t        col,
                    nu_u32_t        fwidth,
                    nu_u32_t        fheight)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_spritesheet(vm, id, texture, row, col, fwidth, fheight);
}
static void
push_scissor (
    wasm_exec_env_t env, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_scissor(vm, x, y, w, h);
}
static void
push_viewport (
    wasm_exec_env_t env, nu_u32_t x, nu_u32_t y, nu_u32_t w, nu_u32_t h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_viewport(vm, x, y, w, h);
}
static void
push_camera (wasm_exec_env_t env, nu_u32_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_camera(vm, id);
}
static void
push_translation (wasm_exec_env_t env, nu_f32_t x, nu_f32_t y, nu_f32_t z)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_translation(vm, x, y, z);
}
static void
push_cursor (wasm_exec_env_t env, nu_u32_t x, nu_u32_t y)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_cursor(vm, x, y);
}
static void
push_color (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_color(vm, color);
}
static void
clear (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_clear(vm, color);
}
static void
draw_model (wasm_exec_env_t env, nu_u32_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_model(vm, id);
}
static void
draw_volume (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_volume(vm);
}
static void
draw_cube (wasm_exec_env_t env, const nu_f32_t *c, const nu_f32_t *s)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_cube(vm, c, s);
}
static void
draw_lines (wasm_exec_env_t env, const nu_f32_t *p, nu_u32_t n)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_lines(vm, p, n);
}
static void
draw_linestrip (wasm_exec_env_t env, const nu_f32_t *p, nu_u32_t n)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_linestrip(vm, p, n);
}
static void
draw_text (wasm_exec_env_t env, const void *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_text(vm, text);
}
static void
print (wasm_exec_env_t env, const void *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_print(vm, text);
}
static void
blit (wasm_exec_env_t env,
      nu_u32_t        id,
      nu_u32_t        x,
      nu_u32_t        y,
      nu_u32_t        w,
      nu_u32_t        h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_blit(vm, id, x, y, w, h);
}
static void
draw_sprite (wasm_exec_env_t env, nu_u32_t spritesheet, nu_u32_t sprite)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_sprite(vm, spritesheet, sprite);
}
static nu_u32_t
button (wasm_exec_env_t env, nu_u32_t player)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_button(vm, player);
}
static nu_f32_t
axis (wasm_exec_env_t env, nu_u32_t player, sys_axis_t axis)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_axis(vm, player, axis);
}

static NativeSymbol wasm_native_symbols[]
    = { EXPORT_WASM_API_WITH_SIG(trace, "(*)"),
        EXPORT_WASM_API_WITH_SIG(inspect_i32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(inspect_f32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(console_info, "(i)i"),
        EXPORT_WASM_API_WITH_SIG(global_time, "()f"),
        EXPORT_WASM_API_WITH_SIG(delta_time, "()f"),
        EXPORT_WASM_API_WITH_SIG(create_scope, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(rewind_scope, "(i)"),
        EXPORT_WASM_API_WITH_SIG(set_active_scope, "(i)"),
        EXPORT_WASM_API_WITH_SIG(create_camera, "(i)"),
        EXPORT_WASM_API_WITH_SIG(set_camera_view, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(set_camera_projection, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(set_camera_lookat, "(i***)"),
        EXPORT_WASM_API_WITH_SIG(set_camera_perspective, "(ifff)"),
        EXPORT_WASM_API_WITH_SIG(create_texture, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(update_texture, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(create_mesh, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(update_mesh, "(iiii*)"),
        EXPORT_WASM_API_WITH_SIG(create_model, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(update_model, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(create_spritesheet, "(iiiiii)"),
        EXPORT_WASM_API_WITH_SIG(push_scissor, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(push_viewport, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(push_camera, "(i)"),
        EXPORT_WASM_API_WITH_SIG(push_translation, "(fff)"),
        EXPORT_WASM_API_WITH_SIG(push_cursor, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(push_color, "(i)"),
        EXPORT_WASM_API_WITH_SIG(clear, "(i)"),
        EXPORT_WASM_API_WITH_SIG(draw_model, "(i)"),
        EXPORT_WASM_API_WITH_SIG(draw_volume, "()"),
        EXPORT_WASM_API_WITH_SIG(draw_cube, "(**)"),
        EXPORT_WASM_API_WITH_SIG(draw_lines, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(draw_linestrip, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(draw_text, "(*)"),
        EXPORT_WASM_API_WITH_SIG(print, "(*)"),
        EXPORT_WASM_API_WITH_SIG(blit, "(iiiii)"),
        EXPORT_WASM_API_WITH_SIG(draw_sprite, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(button, "(i)i"),
        EXPORT_WASM_API_WITH_SIG(axis, "(ii)f") };