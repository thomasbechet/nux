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
static nu_status_t
allocgpool (wasm_exec_env_t env, nu_u32_t index, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_allocgpool(vm, index, size);
}
static nu_status_t
gpool (wasm_exec_env_t env, nu_u32_t index)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_gpool(vm, index);
}
static nu_status_t
cleargpool (wasm_exec_env_t env, nu_u32_t index)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_cleargpool(vm, index);
}
static nu_status_t
alloctex (wasm_exec_env_t env, nu_u32_t idx, nu_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_alloctex(vm, idx, size);
}
static nu_status_t
writetex (wasm_exec_env_t env,
          nu_u32_t        idx,
          nu_u32_t        x,
          nu_u32_t        y,
          nu_u32_t        w,
          nu_u32_t        h,
          const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_writetex(vm, idx, x, y, w, h, p);
}
static nu_status_t
allocmesh (wasm_exec_env_t        env,
           nu_u32_t               idx,
           nu_u32_t               count,
           sys_primitive_t        primitive,
           sys_vertex_attribute_t attribs)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_allocmesh(vm, idx, count, primitive, attribs);
}
static nu_status_t
writemesh (wasm_exec_env_t        env,
           nu_u32_t               idx,
           sys_vertex_attribute_t attribs,
           nu_u32_t               first,
           nu_u32_t               count,
           const void            *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_writemesh(vm, idx, attribs, first, count, p);
}
static nu_status_t
allocmodel (wasm_exec_env_t env, nu_u32_t idx, nu_u32_t count)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_allocmodel(vm, idx, count);
}
static nu_status_t
writemodel (wasm_exec_env_t env,
            nu_u32_t        idx,
            nu_u32_t        node,
            nu_u32_t        mesh,
            nu_u32_t        texture,
            nu_u32_t        parent,
            const nu_f32_t *transform)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_writemodel(vm, idx, node, mesh, texture, parent, transform);
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
fogparams (wasm_exec_env_t env, const nu_f32_t *params)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_fogparams(vm, params);
}
static void
fogcolor (wasm_exec_env_t env, nu_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_fogcolor(vm, color);
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
draw (wasm_exec_env_t env, nu_u32_t index)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw(vm, index);
}
static void
drawc (wasm_exec_env_t env, const nu_f32_t *c, const nu_f32_t *s)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_drawc(vm, c, s);
}
static void
drawl (wasm_exec_env_t env, const nu_f32_t *p, nu_u32_t n)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_drawl(vm, p, n);
}
static void
drawls (wasm_exec_env_t env, const nu_f32_t *p, nu_u32_t n)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_drawls(vm, p, n);
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
      nu_u32_t        index,
      nu_u32_t        x,
      nu_u32_t        y,
      nu_u32_t        w,
      nu_u32_t        h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_blit(vm, index, x, y, w, h);
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
        EXPORT_WASM_API_WITH_SIG(allocgpool, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(gpool, "(i)"),
        EXPORT_WASM_API_WITH_SIG(cleargpool, "(i)"),
        EXPORT_WASM_API_WITH_SIG(alloctex, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(writetex, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(allocmesh, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(writemesh, "(iiii*)"),
        EXPORT_WASM_API_WITH_SIG(allocmodel, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(writemodel, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(transform, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(cursor, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(fogparams, "(*)"),
        EXPORT_WASM_API_WITH_SIG(fogcolor, "(i)"),
        EXPORT_WASM_API_WITH_SIG(clear, "(i)"),
        EXPORT_WASM_API_WITH_SIG(color, "(i)"),
        EXPORT_WASM_API_WITH_SIG(draw, "(i)"),
        EXPORT_WASM_API_WITH_SIG(drawc, "(**)"),
        EXPORT_WASM_API_WITH_SIG(drawl, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(drawls, "(*i)"),
        EXPORT_WASM_API_WITH_SIG(text, "(*)"),
        EXPORT_WASM_API_WITH_SIG(print, "(*)"),
        EXPORT_WASM_API_WITH_SIG(blit, "(iiiii)"),
        EXPORT_WASM_API_WITH_SIG(button, "(i)i"),
        EXPORT_WASM_API_WITH_SIG(axis, "(ii)f") };

#endif
