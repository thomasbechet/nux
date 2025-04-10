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
static nux_object_type_t
object_type (wasm_exec_env_t env, nux_id_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_object_type(vm, id);
}
static nux_status_t
object_put (wasm_exec_env_t env, nux_id_t id, nux_u32_t index)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_object_put(vm, id, index);
}
static nux_id_t
stack_new (wasm_exec_env_t env, nux_id_t stack, nux_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_stack_new(vm, stack, size);
}
static nux_id_t
texture_create (wasm_exec_env_t env, nux_id_t stack, nux_u32_t size)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_texture_create(vm, stack, size);
}
static void
texture_update (wasm_exec_env_t env,
                nux_id_t        id,
                nux_u32_t       x,
                nux_u32_t       y,
                nux_u32_t       w,
                nux_u32_t       h,
                const void     *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_texture_update(vm, id, x, y, w, h, p);
}
static nux_id_t
mesh_create (wasm_exec_env_t        env,
             nux_id_t               stack,
             nux_u32_t              count,
             nux_primitive_t        primitive,
             nux_vertex_attribute_t attributes)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_mesh_create(vm, stack, count, primitive, attributes);
}
static void
mesh_update (wasm_exec_env_t        env,
             nux_id_t               id,
             nux_vertex_attribute_t attributes,
             nux_u32_t              first,
             nux_u32_t              count,
             const void            *p)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_mesh_update(vm, id, attributes, first, count, p);
}
static nux_id_t
spritesheet_create (wasm_exec_env_t env,
                    nux_id_t        stack,
                    nux_id_t        texture,
                    nux_u32_t       row,
                    nux_u32_t       col,
                    nux_u32_t       fwidth,
                    nux_u32_t       fheight)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_spritesheet_create(
        vm, stack, texture, row, col, fwidth, fheight);
}
static nux_id_t
scene_create (wasm_exec_env_t env, nux_id_t stack, nux_u32_t object_capa)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_scene_create(vm, stack, object_capa);
}
static nux_id_t
node_root (wasm_exec_env_t env, nux_id_t scene)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_root(vm, scene);
}
static nux_id_t
node_create (wasm_exec_env_t env, nux_id_t parent)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_create(vm, parent);
}
static nux_id_t
node_create_instance (wasm_exec_env_t env, nux_id_t parent, nux_id_t instance)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_create_instance(vm, parent, instance);
}
static void
node_delete (wasm_exec_env_t env, nux_id_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_delete(vm, id);
}
static void
node_translation (wasm_exec_env_t env, nux_id_t id, nux_f32_t *pos)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_translation(vm, id, pos);
}
static void
node_rotation (wasm_exec_env_t env, nux_id_t id, nux_f32_t *rot)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_rotation(vm, id, rot);
}
static void
node_scale (wasm_exec_env_t env, nux_id_t id, nux_f32_t *scale)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_scale(vm, id, scale);
}
static void
node_set_translation (wasm_exec_env_t env, nux_id_t id, const nux_f32_t *pos)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_set_translation(vm, id, pos);
}
static void
node_set_rotation (wasm_exec_env_t env, nux_id_t id, const nux_f32_t *rot)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_set_rotation(vm, id, rot);
}
static void
node_set_scale (wasm_exec_env_t env, nux_id_t id, const nux_f32_t *scale)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_node_set_scale(vm, id, scale);
}
static nux_id_t
node_scene (wasm_exec_env_t env, nux_id_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_scene(vm, id);
}
static nux_id_t
node_parent (wasm_exec_env_t env, nux_id_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_parent(vm, id);
}
static nux_id_t
node_next (wasm_exec_env_t env, nux_id_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_next(vm, id);
}
static nux_id_t
node_child (wasm_exec_env_t env, nux_id_t id)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_node_child(vm, id);
}
static nux_id_t
camera_create (wasm_exec_env_t env, nux_id_t parent)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_camera_create(vm, parent);
}
static void
camera_set_perspective (wasm_exec_env_t env,
                        nux_id_t        id,
                        nux_f32_t       fov,
                        nux_f32_t       near,
                        nux_f32_t       far)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_camera_set_perspective(vm, id, fov, near, far);
}
static nux_status_t
model_create (wasm_exec_env_t env,
              nux_id_t        parent,
              nux_id_t        mesh,
              nux_id_t        texture)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    return sys_model_create(vm, parent, mesh, texture);
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
      nux_id_t        texture,
      nux_u32_t       x,
      nux_u32_t       y,
      nux_u32_t       w,
      nux_u32_t       h)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_blit(vm, texture, x, y, w, h);
}
static void
draw_sprite (wasm_exec_env_t env, nux_id_t spritesheet, nux_u32_t sprite)
{
    vm_t *vm = wasm_runtime_get_user_data(env);
    sys_draw_sprite(vm, spritesheet, sprite);
}
static void
draw_scene (wasm_exec_env_t env, nux_id_t scene, nux_id_t camera)
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
        EXPORT_WASM_API_WITH_SIG(object_type, "(i)"),
        EXPORT_WASM_API_WITH_SIG(object_put, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(stack_new, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(texture_create, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(texture_update, "(iiiii*)"),
        EXPORT_WASM_API_WITH_SIG(mesh_create, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(mesh_update, "(iiii*)"),
        EXPORT_WASM_API_WITH_SIG(spritesheet_create, "(iiiiii)"),
        EXPORT_WASM_API_WITH_SIG(scene_create, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(node_root, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_create, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_create_instance, "(ii)"),
        EXPORT_WASM_API_WITH_SIG(node_delete, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_translation, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_rotation, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_scale, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_set_translation, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_set_rotation, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_set_scale, "(i*)"),
        EXPORT_WASM_API_WITH_SIG(node_scene, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_parent, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_next, "(i)"),
        EXPORT_WASM_API_WITH_SIG(node_child, "(i)"),
        EXPORT_WASM_API_WITH_SIG(camera_create, "(i)"),
        EXPORT_WASM_API_WITH_SIG(camera_set_perspective, "(iiii)"),
        EXPORT_WASM_API_WITH_SIG(model_create, "(iii)"),
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