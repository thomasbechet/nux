#ifndef WASM_NATIVE_H
#define WASM_NATIVE_H

#include "core/vm.h"
#include <wasm_export.h>

static void
trace (wasm_exec_env_t env, const nu_char_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_trace(vm, text);
}
static nu_u32_t
console_info (wasm_exec_env_t env, sys_console_info_t info)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_console_info(vm, info);
}
static nu_u32_t
add_group (wasm_exec_env_t env, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_add_group(vm, size);
}
static void
clear_group (wasm_exec_env_t env, nu_u32_t group)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_clear_group(vm, group);
}
static nu_status_t
alloc_texture (wasm_exec_env_t env, nu_u32_t id, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_alloc_texture(vm, id, size);
}
static nu_status_t
write_texture (wasm_exec_env_t env,
               nu_u32_t        id,
               nu_u32_t        x,
               nu_u32_t        y,
               nu_u32_t        w,
               nu_u32_t        h,
               const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_write_texture(vm, id, x, y, w, h, p);
}
static nu_status_t
alloc_mesh (wasm_exec_env_t        env,
            nu_u32_t               id,
            nu_u32_t               count,
            sys_primitive_t        primitive,
            sys_vertex_attribute_t attribs)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_alloc_mesh(vm, id, count, primitive, attribs);
}
static nu_status_t
write_mesh (wasm_exec_env_t        env,
            nu_u32_t               id,
            sys_vertex_attribute_t attribs,
            nu_u32_t               first,
            nu_u32_t               count,
            const void            *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_write_mesh(vm, id, attribs, first, count, p);
}
static nu_status_t
alloc_model (wasm_exec_env_t env, nu_u32_t id, nu_u32_t count)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_alloc_model(vm, id, count);
}
static nu_status_t
write_model (wasm_exec_env_t env,
             nu_u32_t        id,
             nu_u32_t        node,
             nu_u32_t        mesh,
             nu_u32_t        texture,
             nu_u32_t        parent,
             const nu_f32_t *transform)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_write_model(vm, id, node, mesh, texture, parent, transform);
}
static nu_status_t
set_spritesheet (wasm_exec_env_t env,
                 nu_u32_t        id,
                 nu_u32_t        texture,
                 nu_u32_t        row,
                 nu_u32_t        col,
                 nu_u32_t        fwidth,
                 nu_u32_t        fheight)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_set_spritesheet(vm, id, texture, row, col, fwidth, fheight);
}
static void
transform (wasm_exec_env_t env, sys_transform_t transform, const nu_f32_t *m)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_transform(vm, transform, m);
}
static void
cursor (wasm_exec_env_t env, nu_u32_t x, nu_u32_t y)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_cursor(vm, x, y);
}
static void
fog_params (wasm_exec_env_t env, const nu_f32_t *params)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_fog_params(vm, params);
}
static void
fog_color (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_fog_color(vm, color);
}
static void
clear (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_clear(vm, color);
}
static void
color (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_color(vm, color);
}
static void
draw (wasm_exec_env_t env, nu_u32_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw(vm, id);
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
text (wasm_exec_env_t env, const void *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_text(vm, text);
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
sprite (wasm_exec_env_t env, nu_u32_t spritesheet, nu_u32_t sprite)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_sprite(vm, spritesheet, sprite);
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
        EXPORT_WASM_API_WITH_SIG(console_info, "(i)i"),
        EXPORT_WASM_API_WITH_SIG(add_group, "(i)i"),
        EXPORT_WASM_API_WITH_SIG(clear_group, "(i)"),
        EXPORT_WASM_API_WITH_SIG(alloc_texture, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(write_texture, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(alloc_mesh, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(write_mesh, "(iiii*)"),
        EXPORT_WASM_API_WITH_SIG(alloc_model, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(write_model, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(set_spritesheet, "(iiiiii)"),
        EXPORT_WASM_API_WITH_SIG(transform, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(cursor, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(fog_params, "(*)"),
        EXPORT_WASM_API_WITH_SIG(fog_color, "(i)"),
        EXPORT_WASM_API_WITH_SIG(clear, "(i)"),
        EXPORT_WASM_API_WITH_SIG(color, "(i)"),
        EXPORT_WASM_API_WITH_SIG(draw, "(i)"),
        EXPORT_WASM_API_WITH_SIG(draw_cube, "(**)"),
        EXPORT_WASM_API_WITH_SIG(draw_lines, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(draw_linestrip, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(text, "(*)"),
        EXPORT_WASM_API_WITH_SIG(print, "(*)"),
        EXPORT_WASM_API_WITH_SIG(blit, "(iiiii)"),
        EXPORT_WASM_API_WITH_SIG(sprite, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(button, "(i)i"),
        EXPORT_WASM_API_WITH_SIG(axis, "(ii)f") };

#endif