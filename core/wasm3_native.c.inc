#include "internal.h"
#include "nux_api.h"
#include <wasm3.h>

static m3ApiRawFunction(trace)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE trace\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, text);
    nux_trace(env, text);
    m3ApiSuccess();
}
static m3ApiRawFunction(inspect_i32)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE inspect_i32\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, name);
    m3ApiGetArgMem(nux_i32_t *, p);
    nux_inspect_i32(env, name, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(inspect_f32)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE inspect_f32\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, name);
    m3ApiGetArgMem(nux_f32_t *, p);
    nux_inspect_f32(env, name, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(console_info)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE console_info\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_console_info_t, info);
    m3ApiReturn(nux_console_info(env, info));
    m3ApiSuccess();
}
static m3ApiRawFunction(global_time)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE global_time\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiReturn(nux_global_time(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(delta_time)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE delta_time\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiReturn(nux_delta_time(env));
    m3ApiSuccess();
}
static m3ApiRawFunction(object_type)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE object_type\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_object_type_t);
    m3ApiGetArg(nux_id_t, id);
    m3ApiReturn(nux_object_type(env, id));
    m3ApiSuccess();
}
static m3ApiRawFunction(object_put)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE object_put\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArg(nux_u32_t, index);
    m3ApiReturn(nux_object_put(env, id, index));
    m3ApiSuccess();
}
static m3ApiRawFunction(stack_new)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE stack_new\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, stack);
    m3ApiGetArg(nux_u32_t, size);
    m3ApiReturn(nux_stack_new(env, stack, size));
    m3ApiSuccess();
}
static m3ApiRawFunction(texture_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE texture_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, stack);
    m3ApiGetArg(nux_u32_t, size);
    m3ApiReturn(nux_texture_create(env, stack, size));
    m3ApiSuccess();
}
static m3ApiRawFunction(texture_update)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE texture_update\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArg(nux_u32_t, x);
    m3ApiGetArg(nux_u32_t, y);
    m3ApiGetArg(nux_u32_t, w);
    m3ApiGetArg(nux_u32_t, h);
    m3ApiGetArgMem(const void *, p);
    nux_texture_update(env, id, x, y, w, h, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(mesh_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE mesh_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, stack);
    m3ApiGetArg(nux_u32_t, count);
    m3ApiGetArg(nux_primitive_t, primitive);
    m3ApiGetArg(nux_vertex_attribute_t, attributes);
    m3ApiReturn(nux_mesh_create(env, stack, count, primitive, attributes));
    m3ApiSuccess();
}
static m3ApiRawFunction(mesh_update)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE mesh_update\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArg(nux_vertex_attribute_t, attributes);
    m3ApiGetArg(nux_u32_t, first);
    m3ApiGetArg(nux_u32_t, count);
    m3ApiGetArgMem(const void *, p);
    nux_mesh_update(env, id, attributes, first, count, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(spritesheet_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE spritesheet_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, stack);
    m3ApiGetArg(nux_id_t, texture);
    m3ApiGetArg(nux_u32_t, row);
    m3ApiGetArg(nux_u32_t, col);
    m3ApiGetArg(nux_u32_t, fwidth);
    m3ApiGetArg(nux_u32_t, fheight);
    m3ApiReturn(
        nux_spritesheet_create(env, stack, texture, row, col, fwidth, fheight));
    m3ApiSuccess();
}
static m3ApiRawFunction(scene_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE scene_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, stack);
    m3ApiGetArg(nux_u32_t, object_capa);
    m3ApiReturn(nux_scene_create(env, stack, object_capa));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_root)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_root\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, scene);
    m3ApiReturn(nux_node_root(env, scene));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, parent);
    m3ApiReturn(nux_node_create(env, parent));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_create_instance)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_create_instance\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, parent);
    m3ApiGetArg(nux_id_t, instance);
    m3ApiReturn(nux_node_create_instance(env, parent, instance));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_delete)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_delete\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    nux_node_delete(env, id);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_translation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_translation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArgMem(nux_f32_t *, pos);
    nux_node_translation(env, id, pos);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_rotation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_rotation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArgMem(nux_f32_t *, rot);
    nux_node_rotation(env, id, rot);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_scale)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_scale\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArgMem(nux_f32_t *, scale);
    nux_node_scale(env, id, scale);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_set_translation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_set_translation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArgMem(const nux_f32_t *, pos);
    nux_node_set_translation(env, id, pos);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_set_rotation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_set_rotation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArgMem(const nux_f32_t *, rot);
    nux_node_set_rotation(env, id, rot);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_set_scale)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_set_scale\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArgMem(const nux_f32_t *, scale);
    nux_node_set_scale(env, id, scale);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_scene)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_scene\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, id);
    m3ApiReturn(nux_node_scene(env, id));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_parent)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_parent\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, id);
    m3ApiReturn(nux_node_parent(env, id));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_next)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_next\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, id);
    m3ApiReturn(nux_node_next(env, id));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_child)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_child\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, id);
    m3ApiReturn(nux_node_child(env, id));
    m3ApiSuccess();
}
static m3ApiRawFunction(camera_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE camera_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_id_t);
    m3ApiGetArg(nux_id_t, parent);
    m3ApiReturn(nux_camera_create(env, parent));
    m3ApiSuccess();
}
static m3ApiRawFunction(camera_set_perspective)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE camera_set_perspective\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, id);
    m3ApiGetArg(nux_f32_t, fov);
    m3ApiGetArg(nux_f32_t, near);
    m3ApiGetArg(nux_f32_t, far);
    nux_camera_set_perspective(env, id, fov, near, far);
    m3ApiSuccess();
}
static m3ApiRawFunction(model_create)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE model_create\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_id_t, parent);
    m3ApiGetArg(nux_id_t, mesh);
    m3ApiGetArg(nux_id_t, texture);
    m3ApiReturn(nux_model_create(env, parent, mesh, texture));
    m3ApiSuccess();
}
static m3ApiRawFunction(push_scissor)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE push_scissor\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u32_t, x);
    m3ApiGetArg(nux_u32_t, y);
    m3ApiGetArg(nux_u32_t, w);
    m3ApiGetArg(nux_u32_t, h);
    nux_push_scissor(env, x, y, w, h);
    m3ApiSuccess();
}
static m3ApiRawFunction(push_viewport)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE push_viewport\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u32_t, x);
    m3ApiGetArg(nux_u32_t, y);
    m3ApiGetArg(nux_u32_t, w);
    m3ApiGetArg(nux_u32_t, h);
    nux_push_viewport(env, x, y, w, h);
    m3ApiSuccess();
}
static m3ApiRawFunction(push_cursor)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE push_cursor\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u32_t, x);
    m3ApiGetArg(nux_u32_t, y);
    nux_push_cursor(env, x, y);
    m3ApiSuccess();
}
static m3ApiRawFunction(push_color)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE push_color\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u32_t, color);
    nux_push_color(env, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(clear)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE clear\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_u32_t, color);
    nux_clear(env, color);
    m3ApiSuccess();
}
static m3ApiRawFunction(draw_text)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE draw_text\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, text);
    nux_draw_text(env, text);
    m3ApiSuccess();
}
static m3ApiRawFunction(print)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE print\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArgMem(const nux_c8_t *, text);
    nux_print(env, text);
    m3ApiSuccess();
}
static m3ApiRawFunction(blit)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE blit\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, texture);
    m3ApiGetArg(nux_u32_t, x);
    m3ApiGetArg(nux_u32_t, y);
    m3ApiGetArg(nux_u32_t, w);
    m3ApiGetArg(nux_u32_t, h);
    nux_blit(env, texture, x, y, w, h);
    m3ApiSuccess();
}
static m3ApiRawFunction(draw_sprite)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE draw_sprite\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, spritesheet);
    m3ApiGetArg(nux_u32_t, sprite);
    nux_draw_sprite(env, spritesheet, sprite);
    m3ApiSuccess();
}
static m3ApiRawFunction(draw_scene)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE draw_scene\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_id_t, scene);
    m3ApiGetArg(nux_id_t, camera);
    nux_draw_scene(env, scene, camera);
    m3ApiSuccess();
}
static m3ApiRawFunction(button)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE button\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_u32_t, player);
    m3ApiReturn(nux_button(env, player));
    m3ApiSuccess();
}
static m3ApiRawFunction(axis)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE axis\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_f32_t);
    m3ApiGetArg(nux_u32_t, player);
    m3ApiGetArg(nux_axis_t, axis);
    m3ApiReturn(nux_axis(env, player, axis));
    m3ApiSuccess();
}

static void
link_raw_functions (IM3Module module)
{
    m3_LinkRawFunction(module, "*", "trace", "v(*)", trace);
    m3_LinkRawFunction(module, "*", "inspect_i32", "v(**)", inspect_i32);
    m3_LinkRawFunction(module, "*", "inspect_f32", "v(**)", inspect_f32);
    m3_LinkRawFunction(module, "*", "console_info", "i(i)", console_info);
    m3_LinkRawFunction(module, "*", "global_time", "f()", global_time);
    m3_LinkRawFunction(module, "*", "delta_time", "f()", delta_time);
    m3_LinkRawFunction(module, "*", "object_type", "(i)", object_type);
    m3_LinkRawFunction(module, "*", "object_put", "v(ii)", object_put);
    m3_LinkRawFunction(module, "*", "stack_new", "i(ii)", stack_new);
    m3_LinkRawFunction(module, "*", "texture_create", "i(ii)", texture_create);
    m3_LinkRawFunction(
        module, "*", "texture_update", "v(iiiii*)", texture_update);
    m3_LinkRawFunction(module, "*", "mesh_create", "i(iiii)", mesh_create);
    m3_LinkRawFunction(module, "*", "mesh_update", "v(iiii*)", mesh_update);
    m3_LinkRawFunction(
        module, "*", "spritesheet_create", "i(iiiiii)", spritesheet_create);
    m3_LinkRawFunction(module, "*", "scene_create", "i(ii)", scene_create);
    m3_LinkRawFunction(module, "*", "node_root", "i(i)", node_root);
    m3_LinkRawFunction(module, "*", "node_create", "i(i)", node_create);
    m3_LinkRawFunction(
        module, "*", "node_create_instance", "i(ii)", node_create_instance);
    m3_LinkRawFunction(module, "*", "node_delete", "v(i)", node_delete);
    m3_LinkRawFunction(
        module, "*", "node_translation", "v(i*)", node_translation);
    m3_LinkRawFunction(module, "*", "node_rotation", "v(i*)", node_rotation);
    m3_LinkRawFunction(module, "*", "node_scale", "v(i*)", node_scale);
    m3_LinkRawFunction(
        module, "*", "node_set_translation", "v(i*)", node_set_translation);
    m3_LinkRawFunction(
        module, "*", "node_set_rotation", "v(i*)", node_set_rotation);
    m3_LinkRawFunction(module, "*", "node_set_scale", "v(i*)", node_set_scale);
    m3_LinkRawFunction(module, "*", "node_scene", "i(i)", node_scene);
    m3_LinkRawFunction(module, "*", "node_parent", "i(i)", node_parent);
    m3_LinkRawFunction(module, "*", "node_next", "i(i)", node_next);
    m3_LinkRawFunction(module, "*", "node_child", "i(i)", node_child);
    m3_LinkRawFunction(module, "*", "camera_create", "i(i)", camera_create);
    m3_LinkRawFunction(module,
                       "*",
                       "camera_set_perspective",
                       "v(ifff)",
                       camera_set_perspective);
    m3_LinkRawFunction(module, "*", "model_create", "v(iii)", model_create);
    m3_LinkRawFunction(module, "*", "push_scissor", "v(iiii)", push_scissor);
    m3_LinkRawFunction(module, "*", "push_viewport", "v(iiii)", push_viewport);
    m3_LinkRawFunction(module, "*", "push_cursor", "v(ii)", push_cursor);
    m3_LinkRawFunction(module, "*", "push_color", "v(i)", push_color);
    m3_LinkRawFunction(module, "*", "clear", "v(i)", clear);
    m3_LinkRawFunction(module, "*", "draw_text", "v(*)", draw_text);
    m3_LinkRawFunction(module, "*", "print", "v(*)", print);
    m3_LinkRawFunction(module, "*", "blit", "v(iiiii)", blit);
    m3_LinkRawFunction(module, "*", "draw_sprite", "v(ii)", draw_sprite);
    m3_LinkRawFunction(module, "*", "draw_scene", "v(ii)", draw_scene);
    m3_LinkRawFunction(module, "*", "button", "i(i)", button);
    m3_LinkRawFunction(module, "*", "axis", "f(ii)", axis);
}