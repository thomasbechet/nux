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
static m3ApiRawFunction(create_scope)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE create_scope\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_u32_t, size);
    m3ApiReturn(nux_create_scope(env, oid, size));
    m3ApiSuccess();
}
static m3ApiRawFunction(rewind_scope)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE rewind_scope\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_oid_t, oid);
    nux_rewind_scope(env, oid);
    m3ApiSuccess();
}
static m3ApiRawFunction(set_active_scope)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE set_active_scope\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_oid_t, oid);
    nux_set_active_scope(env, oid);
    m3ApiSuccess();
}
static m3ApiRawFunction(create_texture)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE create_texture\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_u32_t, size);
    m3ApiReturn(nux_texture_create(env, oid, size));
    m3ApiSuccess();
}
static m3ApiRawFunction(update_texture)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE update_texture\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_u32_t, x);
    m3ApiGetArg(nux_u32_t, y);
    m3ApiGetArg(nux_u32_t, w);
    m3ApiGetArg(nux_u32_t, h);
    m3ApiGetArgMem(const void *, p);
    nux_texture_update(env, oid, x, y, w, h, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(create_mesh)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE create_mesh\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_u32_t, count);
    m3ApiGetArg(nux_primitive_t, primitive);
    m3ApiGetArg(nux_vertex_attribute_t, attributes);
    m3ApiReturn(nux_mesh_create(env, oid, count, primitive, attributes));
    m3ApiSuccess();
}
static m3ApiRawFunction(update_mesh)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE update_mesh\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_vertex_attribute_t, attributes);
    m3ApiGetArg(nux_u32_t, first);
    m3ApiGetArg(nux_u32_t, count);
    m3ApiGetArgMem(const void *, p);
    nux_update_mesh(env, oid, attributes, first, count, p);
    m3ApiSuccess();
}
static m3ApiRawFunction(create_spritesheet)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE create_spritesheet\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_u32_t, texture);
    m3ApiGetArg(nux_u32_t, row);
    m3ApiGetArg(nux_u32_t, col);
    m3ApiGetArg(nux_oid_t, fwidth);
    m3ApiGetArg(nux_u32_t, fheight);
    m3ApiReturn(
        nux_create_spritesheet(env, oid, texture, row, col, fwidth, fheight));
    m3ApiSuccess();
}
static m3ApiRawFunction(create_scene)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE create_scene\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiGetArg(nux_u32_t, node_capa);
    m3ApiReturn(nux_create_scene(env, oid, node_capa));
    m3ApiSuccess();
}
static m3ApiRawFunction(bind_scene)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE bind_scene\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_oid_t, oid);
    m3ApiReturn(nux_bind_scene(env, oid));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_add)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_add\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_nid_t);
    m3ApiGetArg(nux_nid_t, parent);
    m3ApiReturn(nux_create_node(env, parent));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_add_instance)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_add_instance\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_nid_t);
    m3ApiGetArg(nux_nid_t, parent);
    m3ApiGetArg(nux_oid_t, scene);
    m3ApiReturn(nux_create_instance_node(env, parent, scene));
    m3ApiSuccess();
}
static m3ApiRawFunction(node_remove)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_remove\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    nux_delete_node(env, nid);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_get_translation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_get_translation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArgMem(nux_f32_t *, pos);
    nux_get_node_translation(env, nid, pos);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_set_translation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_set_translation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArgMem(const nux_f32_t *, pos);
    nux_set_node_translation(env, nid, pos);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_get_rotation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_get_rotation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArgMem(nux_f32_t *, rot);
    nux_get_node_rotation(env, nid, rot);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_set_rotation)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_set_rotation\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArgMem(const nux_f32_t *, rot);
    nux_set_node_rotation(env, nid, rot);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_get_scale)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_get_scale\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArgMem(nux_f32_t *, scale);
    nux_get_node_scale(env, nid, scale);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_set_scale)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_set_scale\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArgMem(const nux_f32_t *, scale);
    nux_set_node_scale(env, nid, scale);
    m3ApiSuccess();
}
static m3ApiRawFunction(node_get_parent)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE node_get_parent\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_u32_t);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiReturn(nux_node_parent(env, nid));
    m3ApiSuccess();
}
static m3ApiRawFunction(camera_add)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE camera_add\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiReturn(nux_create_camera(env, nid));
    m3ApiSuccess();
}
static m3ApiRawFunction(camera_remove)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE camera_remove\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    nux_delete_camera(env, nid);
    m3ApiSuccess();
}
static m3ApiRawFunction(camera_set_perspective)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE camera_set_perspective\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArg(nux_f32_t, fov);
    m3ApiGetArg(nux_f32_t, near);
    m3ApiGetArg(nux_f32_t, far);
    nux_set_camera_perspective(env, nid, fov, near, far);
    m3ApiSuccess();
}
static m3ApiRawFunction(model_add)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE model_add\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiReturnType(nux_status_t);
    m3ApiGetArg(nux_nid_t, nid);
    m3ApiGetArg(nux_oid_t, mesh);
    m3ApiGetArg(nux_oid_t, texture);
    m3ApiReturn(nux_create_model(env, nid, mesh, texture));
    m3ApiSuccess();
}
static m3ApiRawFunction(model_remove)
{
#ifdef NUX_BUILD_TRACE
    printf("TRACE model_remove\n");
#endif
    nux_env_t env = m3_GetUserData(runtime);
    m3ApiGetArg(nux_nid_t, nid);
    nux_model_remove(env, nid);
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
    m3ApiGetArg(nux_oid_t, texture);
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
    m3ApiGetArg(nux_oid_t, spritesheet);
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
    m3ApiGetArg(nux_oid_t, scene);
    m3ApiGetArg(nux_nid_t, camera);
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
    m3_LinkRawFunction(module, "*", "create_scope", "v(ii)", create_scope);
    m3_LinkRawFunction(module, "*", "rewind_scope", "v(i)", rewind_scope);
    m3_LinkRawFunction(
        module, "*", "set_active_scope", "v(i)", set_active_scope);
    m3_LinkRawFunction(module, "*", "create_texture", "v(ii)", create_texture);
    m3_LinkRawFunction(
        module, "*", "update_texture", "v(iiiii*)", update_texture);
    m3_LinkRawFunction(module, "*", "create_mesh", "v(iiii)", create_mesh);
    m3_LinkRawFunction(module, "*", "update_mesh", "v(iiii*)", update_mesh);
    m3_LinkRawFunction(
        module, "*", "create_spritesheet", "v(iiiiii)", create_spritesheet);
    m3_LinkRawFunction(module, "*", "create_scene", "v(ii)", create_scene);
    m3_LinkRawFunction(module, "*", "bind_scene", "v(i)", bind_scene);
    m3_LinkRawFunction(module, "*", "node_add", "i(i)", node_add);
    m3_LinkRawFunction(
        module, "*", "node_add_instance", "i(ii)", node_add_instance);
    m3_LinkRawFunction(module, "*", "node_remove", "v(i)", node_remove);
    m3_LinkRawFunction(
        module, "*", "node_get_translation", "v(i*)", node_get_translation);
    m3_LinkRawFunction(
        module, "*", "node_set_translation", "v(i*)", node_set_translation);
    m3_LinkRawFunction(
        module, "*", "node_get_rotation", "v(i*)", node_get_rotation);
    m3_LinkRawFunction(
        module, "*", "node_set_rotation", "v(i*)", node_set_rotation);
    m3_LinkRawFunction(module, "*", "node_get_scale", "v(i*)", node_get_scale);
    m3_LinkRawFunction(module, "*", "node_set_scale", "v(i*)", node_set_scale);
    m3_LinkRawFunction(module, "*", "node_get_parent", "i(i)", node_get_parent);
    m3_LinkRawFunction(module, "*", "camera_add", "v(i)", camera_add);
    m3_LinkRawFunction(module, "*", "camera_remove", "v(i)", camera_remove);
    m3_LinkRawFunction(module,
                       "*",
                       "camera_set_perspective",
                       "v(ifff)",
                       camera_set_perspective);
    m3_LinkRawFunction(module, "*", "model_add", "v(iii)", model_add);
    m3_LinkRawFunction(module, "*", "model_remove", "v(i)", model_remove);
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
