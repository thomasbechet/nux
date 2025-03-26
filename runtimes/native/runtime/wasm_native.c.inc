#include "core/vm.h"
#include <wasm_export.h>

static void
trace (wasm_exec_env_t env, const nux_c8_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_trace(vm, text);
}
static void
inspect_i32 (wasm_exec_env_t env, const nux_c8_t *name, nux_i32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_inspect_i32(vm, name, p);
}
static void
inspect_f32 (wasm_exec_env_t env, const nux_c8_t *name, nux_f32_t *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_inspect_f32(vm, name, p);
}
static nux_u32_t
console_info (wasm_exec_env_t env, nux_console_info_t info)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_console_info(vm, info);
}
static nux_f32_t
global_time (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_global_time(vm);
}
static nux_f32_t
delta_time (wasm_exec_env_t env)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_delta_time(vm);
}
static nux_status_t
create_scope (wasm_exec_env_t env, nux_oid_t oid, nux_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_scope(vm, oid, size);
}
static void
rewind_scope (wasm_exec_env_t env, nux_oid_t oid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_rewind_scope(vm, oid);
}
static void
set_active_scope (wasm_exec_env_t env, nux_oid_t oid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_set_active_scope(vm, oid);
}
static nux_status_t
create_texture (wasm_exec_env_t env, nux_oid_t oid, nux_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_texture(vm, oid, size);
}
static void
update_texture (wasm_exec_env_t env,
                nux_oid_t       oid,
                nux_u32_t       x,
                nux_u32_t       y,
                nux_u32_t       w,
                nux_u32_t       h,
                const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_update_texture(vm, oid, x, y, w, h, p);
}
static nux_status_t
create_mesh (wasm_exec_env_t        env,
             nux_oid_t              oid,
             nux_u32_t              count,
             nux_primitive_t        primitive,
             nux_vertex_attribute_t attributes)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_mesh(vm, oid, count, primitive, attributes);
}
static void
update_mesh (wasm_exec_env_t        env,
             nux_oid_t              oid,
             nux_vertex_attribute_t attributes,
             nux_u32_t              first,
             nux_u32_t              count,
             const void            *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_update_mesh(vm, oid, attributes, first, count, p);
}
static nux_status_t
create_spritesheet (wasm_exec_env_t env,
                    nux_oid_t       oid,
                    nux_u32_t       texture,
                    nux_u32_t       row,
                    nux_u32_t       col,
                    nux_oid_t       fwidth,
                    nux_u32_t       fheight)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_spritesheet(vm, oid, texture, row, col, fwidth, fheight);
}
static nux_status_t
create_scene (wasm_exec_env_t env, nux_oid_t oid, nux_u32_t node_capa)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_create_scene(vm, oid, node_capa);
}
static void
bind_scene (wasm_exec_env_t env, nux_oid_t oid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_bind_scene(vm, oid);
}
static nux_nid_t
node_add (wasm_exec_env_t env, nux_nid_t parent)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_add(vm, parent);
}
static void
node_remove (wasm_exec_env_t env, nux_nid_t nid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_remove(vm, nid);
}
static void
node_get_position (wasm_exec_env_t env, nux_nid_t nid, nux_f32_t *pos)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_get_position(vm, nid, pos);
}
static void
node_set_position (wasm_exec_env_t env, nux_nid_t nid, const nux_f32_t *pos)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_set_position(vm, nid, pos);
}
static void
node_get_rotation (wasm_exec_env_t env, nux_nid_t nid, nux_f32_t *rot)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_get_rotation(vm, nid, rot);
}
static void
node_set_rotation (wasm_exec_env_t env, nux_nid_t nid, const nux_f32_t *rot)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_set_rotation(vm, nid, rot);
}
static void
node_get_scale (wasm_exec_env_t env, nux_nid_t nid, nux_f32_t *scale)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_get_scale(vm, nid, scale);
}
static void
node_set_scale (wasm_exec_env_t env, nux_nid_t nid, const nux_f32_t *scale)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_set_scale(vm, nid, scale);
}
static nux_u32_t
node_get_parent (wasm_exec_env_t env, nux_nid_t nid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_get_parent(vm, nid);
}
static nux_status_t
camera_add (wasm_exec_env_t env, nux_nid_t nid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_camera_add(vm, nid);
}
static void
camera_remove (wasm_exec_env_t env, nux_nid_t nid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_camera_remove(vm, nid);
}
static void
camera_set_perspective (wasm_exec_env_t env,
                        nux_nid_t       nid,
                        nux_f32_t       fov,
                        nux_f32_t       near,
                        nux_f32_t       far)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_camera_set_perspective(vm, nid, fov, near, far);
}
static nux_status_t
model_add (wasm_exec_env_t env,
           nux_nid_t       nid,
           nux_oid_t       mesh,
           nux_oid_t       texture)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_model_add(vm, nid, mesh, texture);
}
static void
model_remove (wasm_exec_env_t env, nux_nid_t nid)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_model_remove(vm, nid);
}
static void
push_scissor (
    wasm_exec_env_t env, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_scissor(vm, x, y, w, h);
}
static void
push_viewport (
    wasm_exec_env_t env, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_viewport(vm, x, y, w, h);
}
static void
push_cursor (wasm_exec_env_t env, nux_u32_t x, nux_u32_t y)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_cursor(vm, x, y);
}
static void
push_color (wasm_exec_env_t env, nux_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_push_color(vm, color);
}
static void
clear (wasm_exec_env_t env, nux_u32_t color)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_clear(vm, color);
}
static void
draw_text (wasm_exec_env_t env, const nux_c8_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_text(vm, text);
}
static void
print (wasm_exec_env_t env, const nux_c8_t *text)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_print(vm, text);
}
static void
blit (wasm_exec_env_t env,
      nux_oid_t       texture,
      nux_u32_t       x,
      nux_u32_t       y,
      nux_u32_t       w,
      nux_u32_t       h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_blit(vm, texture, x, y, w, h);
}
static void
draw_sprite (wasm_exec_env_t env, nux_oid_t spritesheet, nux_u32_t sprite)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_sprite(vm, spritesheet, sprite);
}
static void
draw_scene (wasm_exec_env_t env, nux_oid_t scene, nux_nid_t camera)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_scene(vm, scene, camera);
}
static nux_u32_t
button (wasm_exec_env_t env, nux_u32_t player)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_button(vm, player);
}
static nux_f32_t
axis (wasm_exec_env_t env, nux_u32_t player, nux_axis_t axis)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_axis(vm, player, axis);
}

static NativeSymbol wasm_native_symbols[]
    = { EXPORT_WASM_API_WITH_SIG(trace, "(*)"),
        EXPORT_WASM_API_WITH_SIG(inspect_i32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(inspect_f32, "(**)"),
        EXPORT_WASM_API_WITH_SIG(console_info, "(i)"),
        EXPORT_WASM_API_WITH_SIG(global_time, "()"),
        EXPORT_WASM_API_WITH_SIG(delta_time, "()"),
        EXPORT_WASM_API_WITH_SIG(create_scope, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(rewind_scope, "(i)"),
        EXPORT_WASM_API_WITH_SIG(set_active_scope, "(i)"),
        EXPORT_WASM_API_WITH_SIG(create_texture, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(update_texture, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(create_mesh, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(update_mesh, "(iiii*)"),
        EXPORT_WASM_API_WITH_SIG(create_spritesheet, "(iiiiii)"),
        EXPORT_WASM_API_WITH_SIG(create_scene, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(bind_scene, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_add, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_remove, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_get_position, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_set_position, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_get_rotation, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_set_rotation, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_get_scale, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_set_scale, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_get_parent, "(i)"),
        EXPORT_WASM_API_WITH_SIG(camera_add, "(i)"),
        EXPORT_WASM_API_WITH_SIG(camera_remove, "(i)"),
        EXPORT_WASM_API_WITH_SIG(camera_set_perspective, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(model_add, "(iii)"),
        EXPORT_WASM_API_WITH_SIG(model_remove, "(i)"),
        EXPORT_WASM_API_WITH_SIG(push_scissor, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(push_viewport, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(push_cursor, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(push_color, "(i)"),
        EXPORT_WASM_API_WITH_SIG(clear, "(i)"),
        EXPORT_WASM_API_WITH_SIG(draw_text, "(*)"),
        EXPORT_WASM_API_WITH_SIG(print, "(*)"),
        EXPORT_WASM_API_WITH_SIG(blit, "(iiiii)"),
        EXPORT_WASM_API_WITH_SIG(draw_sprite, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(draw_scene, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(button, "(i)"),
        EXPORT_WASM_API_WITH_SIG(axis, "(ii)") };