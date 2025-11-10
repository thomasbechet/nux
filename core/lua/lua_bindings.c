#include "internal.h"

static void
l_checkerror (lua_State *L)
{
    if (!nux_error_status())
    {
        luaL_error(L, nux_error_message());
    }
}

static int
l_stat (lua_State *L)
{
    nux_stat_t info = luaL_checknumber(L, 1);
    nux_u32_t  ret  = nux_stat(info);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_elapsed_time (lua_State *L)
{
    nux_f32_t ret = nux_elapsed_time();
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_delta_time (lua_State *L)
{
    nux_f32_t ret = nux_delta_time();
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_frame_index (lua_State *L)
{
    nux_u32_t ret = nux_frame_index();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_timestamp (lua_State *L)
{
    nux_u64_t ret = nux_timestamp();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_random (lua_State *L)
{
    nux_u32_t ret = nux_random();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_random01 (lua_State *L)
{
    nux_f32_t ret = nux_random01();
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_new_arena (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_arena_t *ret = nux_new_arena(arena);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_clear_arena (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));

    nux_clear_arena(arena);
    l_checkerror(L);

    return 0;
}
static int
l_arena_block_count (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_arena_block_count(arena);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_arena_memory_usage (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_arena_memory_usage(arena);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_set_log_level (lua_State *L)
{
    nux_log_level_t level = luaL_checknumber(L, 1);

    nux_set_log_level(level);
    l_checkerror(L);

    return 0;
}
static int
l_reload_resource (lua_State *L)
{
    nux_rid_t    rid = (nux_rid_t)luaL_checknumber(L, 1);
    nux_status_t ret = nux_reload_resource(rid);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_module_requires (lua_State *L)
{
    const nux_c8_t *name = luaL_checkstring(L, 1);
    nux_status_t    ret  = nux_module_requires(name);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_event_type (lua_State *L)
{
    nux_event_t *event
        = nux_check_resource(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 1));
    nux_event_type_t ret = nux_event_type(event);
    l_checkerror(L);
}
static int
l_process_event (lua_State *L)
{
    nux_event_t *event
        = nux_check_resource(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 1));

    nux_process_event(event);
    l_checkerror(L);

    return 0;
}
static int
l_io_cart_begin (lua_State *L)
{
    const nux_c8_t *path        = luaL_checkstring(L, 1);
    nux_u32_t       entry_count = luaL_checknumber(L, 2);
    nux_status_t    ret         = nux_io_cart_begin(path, entry_count);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_io_write_cart_file (lua_State *L)
{
    const nux_c8_t *path = luaL_checkstring(L, 1);
    nux_status_t    ret  = nux_io_write_cart_file(path);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_mount_disk (lua_State *L)
{
    const nux_c8_t *path = luaL_checkstring(L, 1);
    nux_status_t    ret  = nux_mount_disk(path);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_file_exists (lua_State *L)
{
    const nux_c8_t *path = luaL_checkstring(L, 1);
    nux_b32_t       ret  = nux_file_exists(path);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_open_file (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t *path = luaL_checkstring(L, 2);
    nux_io_mode_t   mode = luaL_checknumber(L, 3);
    nux_file_t     *ret  = nux_open_file(arena, path, mode);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_close_file (lua_State *L)
{
    nux_file_t *file
        = nux_check_resource(NUX_RESOURCE_FILE, luaL_checkinteger(L, 1));

    nux_close_file(file);
    l_checkerror(L);

    return 0;
}
static int
l_seek_file (lua_State *L)
{
    nux_file_t *file
        = nux_check_resource(NUX_RESOURCE_FILE, luaL_checkinteger(L, 1));
    nux_u32_t    cursor = luaL_checknumber(L, 2);
    nux_status_t ret    = nux_seek_file(file, cursor);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_file_size (lua_State *L)
{
    nux_file_t *file
        = nux_check_resource(NUX_RESOURCE_FILE, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_file_size(file);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_new_inputmap (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_inputmap_t *ret = nux_new_inputmap(arena);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_bind_key (lua_State *L)
{
    nux_inputmap_t *map
        = nux_check_resource(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 1));
    const nux_c8_t *name = luaL_checkstring(L, 2);
    nux_key_t       key  = luaL_checknumber(L, 3);

    nux_bind_key(map, name, key);
    l_checkerror(L);

    return 0;
}
static int
l_bind_mouse_button (lua_State *L)
{
    nux_inputmap_t *map
        = nux_check_resource(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 1));
    const nux_c8_t    *name   = luaL_checkstring(L, 2);
    nux_mouse_button_t button = luaL_checknumber(L, 3);

    nux_bind_mouse_button(map, name, button);
    l_checkerror(L);

    return 0;
}
static int
l_bind_mouse_axis (lua_State *L)
{
    nux_inputmap_t *map
        = nux_check_resource(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 1));
    const nux_c8_t  *name      = luaL_checkstring(L, 2);
    nux_mouse_axis_t axis      = luaL_checknumber(L, 3);
    nux_f32_t        sensivity = luaL_checknumber(L, 4);

    nux_bind_mouse_axis(map, name, axis, sensivity);
    l_checkerror(L);

    return 0;
}
static int
l_set_input_map (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    nux_inputmap_t *map
        = nux_check_resource(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 2));

    nux_set_input_map(controller, map);
    l_checkerror(L);

    return 0;
}
static int
l_is_pressed (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_is_pressed(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_is_released (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_is_released(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_is_just_pressed (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_is_just_pressed(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_is_just_released (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_is_just_released(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_axis (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_f32_t       ret        = nux_axis(controller, name);
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_cursor (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_v2_t  ret        = nux_cursor(controller);
    l_checkerror(L);

    nux_lua_push_vec2(L, ret);
    return 1;
}
static int
l_wrap_cursor (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_f32_t x          = luaL_checknumber(L, 2);
    nux_f32_t y          = luaL_checknumber(L, 3);

    nux_wrap_cursor(controller, x, y);
    l_checkerror(L);

    return 0;
}
static int
l_lua_load (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t *path = luaL_checkstring(L, 2);
    nux_lua_t      *ret  = nux_lua_load(arena, path);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_transform_matrix (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_m4_t  ret = nux_transform_matrix(e);
    l_checkerror(L);

    nux_lua_push_mat4(L, ret);
    return 1;
}
static int
l_transform_local_translation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_local_translation(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_local_rotation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_q4_t  ret = nux_transform_local_rotation(e);
    l_checkerror(L);

    nux_lua_push_q4(L, ret);
    return 1;
}
static int
l_transform_local_scale (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_local_scale(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_translation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_translation(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_rotation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_q4_t  ret = nux_transform_rotation(e);
    l_checkerror(L);

    nux_lua_push_q4(L, ret);
    return 1;
}
static int
l_transform_scale (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_scale(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_set_translation (lua_State *L)
{
    nux_nid_t e        = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  position = nux_lua_check_vec3(L, 2);

    nux_transform_set_translation(e, position);
    l_checkerror(L);

    return 0;
}
static int
l_transform_set_rotation (lua_State *L)
{
    nux_nid_t e        = (nux_nid_t)luaL_checknumber(L, 1);
    nux_q4_t  rotation = nux_lua_check_q4(L, 2);

    nux_transform_set_rotation(e, rotation);
    l_checkerror(L);

    return 0;
}
static int
l_transform_set_rotation_euler (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  euler = nux_lua_check_vec3(L, 2);

    nux_transform_set_rotation_euler(e, euler);
    l_checkerror(L);

    return 0;
}
static int
l_transform_set_scale (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  scale = nux_lua_check_vec3(L, 2);

    nux_transform_set_scale(e, scale);
    l_checkerror(L);

    return 0;
}
static int
l_transform_set_ortho (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  a = nux_lua_check_vec3(L, 2);
    nux_v3_t  b = nux_lua_check_vec3(L, 3);
    nux_v3_t  c = nux_lua_check_vec3(L, 4);

    nux_transform_set_ortho(e, a, b, c);
    l_checkerror(L);

    return 0;
}
static int
l_transform_forward (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_forward(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_backward (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_backward(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_left (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_left(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_right (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_right(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_up (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_up(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_down (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_down(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_rotate (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  axis  = nux_lua_check_vec3(L, 2);
    nux_f32_t angle = luaL_checknumber(L, 3);

    nux_transform_rotate(e, axis, angle);
    l_checkerror(L);

    return 0;
}
static int
l_transform_rotate_x (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_x(e, angle);
    l_checkerror(L);

    return 0;
}
static int
l_transform_rotate_y (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_y(e, angle);
    l_checkerror(L);

    return 0;
}
static int
l_transform_rotate_z (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_z(e, angle);
    l_checkerror(L);

    return 0;
}
static int
l_transform_look_at (lua_State *L)
{
    nux_nid_t e      = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  center = nux_lua_check_vec3(L, 2);

    nux_transform_look_at(e, center);
    l_checkerror(L);

    return 0;
}
static int
l_query_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t    include_count = luaL_checknumber(L, 2);
    nux_u32_t    exclude_count = luaL_checknumber(L, 3);
    nux_query_t *ret = nux_query_new(arena, include_count, exclude_count);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_query_new_any (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_query_t *ret = nux_query_new_any(arena);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_query_includes (lua_State *L)
{
    nux_query_t *it
        = nux_check_resource(NUX_RESOURCE_QUERY, luaL_checkinteger(L, 1));
    nux_u32_t c = luaL_checknumber(L, 2);

    nux_query_includes(it, c);
    l_checkerror(L);

    return 0;
}
static int
l_query_excludes (lua_State *L)
{
    nux_query_t *it
        = nux_check_resource(NUX_RESOURCE_QUERY, luaL_checkinteger(L, 1));
    nux_u32_t c = luaL_checknumber(L, 2);

    nux_query_excludes(it, c);
    l_checkerror(L);

    return 0;
}
static int
l_query_next (lua_State *L)
{
    nux_query_t *it
        = nux_check_resource(NUX_RESOURCE_QUERY, luaL_checkinteger(L, 1));
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 2);
    nux_u32_t ret = nux_query_next(it, e);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_scene_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_scene_t *ret = nux_scene_new(arena);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_scene_set_active (lua_State *L)
{
    nux_scene_t *scene
        = nux_check_resource(NUX_RESOURCE_SCENE, luaL_checkinteger(L, 1));
    nux_status_t ret = nux_scene_set_active(scene);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_node_create (lua_State *L)
{
    nux_nid_t parent = (nux_nid_t)luaL_checknumber(L, 1);
    nux_nid_t ret    = nux_node_create(parent);
    l_checkerror(L);

    if (ret)
    {
        lua_pushinteger(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_node_delete (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_node_delete(e);
    l_checkerror(L);

    return 0;
}
static int
l_node_valid (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_b32_t ret = nux_node_valid(e);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_node_parent (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_nid_t ret = nux_node_parent(e);
    l_checkerror(L);

    if (ret)
    {
        lua_pushinteger(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_node_set_parent (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_nid_t p = (nux_nid_t)luaL_checknumber(L, 2);

    nux_node_set_parent(e, p);
    l_checkerror(L);

    return 0;
}
static int
l_node_sibling (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_nid_t ret = nux_node_sibling(e);
    l_checkerror(L);

    if (ret)
    {
        lua_pushinteger(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_node_child (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_nid_t ret = nux_node_child(e);
    l_checkerror(L);

    if (ret)
    {
        lua_pushinteger(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_node_add (lua_State *L)
{
    nux_nid_t n = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t c = luaL_checknumber(L, 2);

    nux_node_add(n, c);
    l_checkerror(L);

    return 0;
}
static int
l_node_remove (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t c = luaL_checknumber(L, 2);

    nux_node_remove(e, c);
    l_checkerror(L);

    return 0;
}
static int
l_node_has (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t c   = luaL_checknumber(L, 2);
    nux_b32_t ret = nux_node_has(e, c);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_node_instantiate (lua_State *L)
{
    nux_scene_t *scene
        = nux_check_resource(NUX_RESOURCE_SCENE, luaL_checkinteger(L, 1));
    nux_nid_t parent = (nux_nid_t)luaL_checknumber(L, 2);
    nux_nid_t ret    = nux_node_instantiate(scene, parent);
    l_checkerror(L);

    if (ret)
    {
        lua_pushinteger(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_scene_load_gltf (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t *path = luaL_checkstring(L, 2);
    nux_scene_t    *ret  = nux_scene_load_gltf(arena, path);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_viewport_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_texture_t *target
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));
    nux_viewport_t *ret = nux_viewport_new(arena, target);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_viewport_set_mode (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_viewport_mode_t mode = luaL_checknumber(L, 2);

    nux_viewport_set_mode(vp, mode);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_extent (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_b2i_t extent = nux_lua_check_box2i(L, 2);

    nux_viewport_set_extent(vp, extent);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_anchor (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_u32_t anchor = luaL_checknumber(L, 2);

    nux_viewport_set_anchor(vp, anchor);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_layer (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_i32_t layer = luaL_checknumber(L, 2);

    nux_viewport_set_layer(vp, layer);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_clear_depth (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_b32_t clear = lua_toboolean(L, 2);

    nux_viewport_set_clear_depth(vp, clear);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_auto_resize (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_b32_t enable = lua_toboolean(L, 2);

    nux_viewport_set_auto_resize(vp, enable);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_camera (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_nid_t camera = (nux_nid_t)luaL_checknumber(L, 2);

    nux_viewport_set_camera(vp, camera);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_texture (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_texture_t *texture
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));

    nux_viewport_set_texture(vp, texture);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_get_normalized_viewport (lua_State *L)
{
    nux_viewport_t *viewport
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_v4_t ret = nux_viewport_get_normalized_viewport(viewport);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
    return 1;
}
static int
l_viewport_to_global (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_v2_t coord = nux_lua_check_vec2(L, 2);
    nux_v2_t ret   = nux_viewport_to_global(vp, coord);
    l_checkerror(L);

    nux_lua_push_vec2(L, ret);
    return 1;
}
static int
l_viewport_to_local (lua_State *L)
{
    nux_viewport_t *vp
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_v2_t coord = nux_lua_check_vec2(L, 2);
    nux_v2_t ret   = nux_viewport_to_local(vp, coord);
    l_checkerror(L);

    nux_lua_push_vec2(L, ret);
    return 1;
}
static int
l_texture_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_texture_type_t format = luaL_checknumber(L, 2);
    nux_u32_t          w      = luaL_checknumber(L, 3);
    nux_u32_t          h      = luaL_checknumber(L, 4);
    nux_texture_t     *ret    = nux_texture_new(arena, format, w, h);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_texture_load (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t *path = luaL_checkstring(L, 2);
    nux_texture_t  *ret  = nux_texture_load(arena, path);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_texture_get_size (lua_State *L)
{
    nux_texture_t *texture
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 1));
    nux_v2i_t ret = nux_texture_get_size(texture);
    l_checkerror(L);

    nux_lua_push_vec2i(L, ret);
    return 1;
}
static int
l_palette_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t      size = luaL_checknumber(L, 2);
    nux_palette_t *ret  = nux_palette_new(arena, size);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_palette_set_active (lua_State *L)
{
    nux_palette_t *palette
        = nux_check_resource(NUX_RESOURCE_PALETTE, luaL_checkinteger(L, 1));

    nux_palette_set_active(palette);
    l_checkerror(L);

    return 0;
}
static int
l_palette_set_color (lua_State *L)
{
    nux_palette_t *palette
        = nux_check_resource(NUX_RESOURCE_PALETTE, luaL_checkinteger(L, 1));
    nux_u32_t index = luaL_checknumber(L, 2);
    nux_v4_t  color = nux_lua_check_vec4(L, 3);

    nux_palette_set_color(palette, index, color);
    l_checkerror(L);

    return 0;
}
static int
l_palette_get_color (lua_State *L)
{
    nux_palette_t *palette
        = nux_check_resource(NUX_RESOURCE_PALETTE, luaL_checkinteger(L, 1));
    nux_u32_t index = luaL_checknumber(L, 2);
    nux_v4_t  ret   = nux_palette_get_color(palette, index);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
    return 1;
}
static int
l_mesh_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t              capa       = luaL_checknumber(L, 2);
    nux_vertex_attribute_t attributes = luaL_checknumber(L, 3);
    nux_vertex_primitive_t primitive  = luaL_checknumber(L, 4);
    nux_mesh_t *ret = nux_mesh_new(arena, capa, attributes, primitive);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_mesh_new_cube (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_f32_t   sx  = luaL_checknumber(L, 2);
    nux_f32_t   sy  = luaL_checknumber(L, 3);
    nux_f32_t   sz  = luaL_checknumber(L, 4);
    nux_mesh_t *ret = nux_mesh_new_cube(arena, sx, sy, sz);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_mesh_new_plane (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_f32_t   sx  = luaL_checknumber(L, 2);
    nux_f32_t   sy  = luaL_checknumber(L, 3);
    nux_mesh_t *ret = nux_mesh_new_plane(arena, sx, sy);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_mesh_update_bounds (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));

    nux_mesh_update_bounds(mesh);
    l_checkerror(L);

    return 0;
}
static int
l_mesh_bounds_min (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_v3_t ret = nux_mesh_bounds_min(mesh);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_mesh_bounds_max (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_v3_t ret = nux_mesh_bounds_max(mesh);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_mesh_set_origin (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_v3_t origin = nux_lua_check_vec3(L, 2);

    nux_mesh_set_origin(mesh, origin);
    l_checkerror(L);

    return 0;
}
static int
l_mesh_transform (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_m4_t transform = nux_lua_check_mat4(L, 2);

    nux_mesh_transform(mesh, transform);
    l_checkerror(L);

    return 0;
}
static int
l_mesh_size (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_mesh_size(mesh);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_canvas_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t     width  = luaL_checknumber(L, 2);
    nux_u32_t     height = luaL_checknumber(L, 3);
    nux_canvas_t *ret    = nux_canvas_new(arena, width, height);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_canvas_get_texture (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_t *ret = nux_canvas_get_texture(canvas);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_canvas_get_size (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_v2i_t ret = nux_canvas_get_size(canvas);
    l_checkerror(L);

    nux_lua_push_vec2i(L, ret);
    return 1;
}
static int
l_canvas_set_clear_color (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_u32_t color = luaL_checknumber(L, 2);

    nux_canvas_set_clear_color(canvas, color);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_set_wrap_mode (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_wrap_mode_t mode = luaL_checknumber(L, 2);

    nux_canvas_set_wrap_mode(canvas, mode);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_text (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_u32_t       x    = luaL_checknumber(L, 2);
    nux_u32_t       y    = luaL_checknumber(L, 3);
    const nux_c8_t *text = luaL_checkstring(L, 4);

    nux_canvas_text(canvas, x, y, text);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_rectangle (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_u32_t x = luaL_checknumber(L, 2);
    nux_u32_t y = luaL_checknumber(L, 3);
    nux_u32_t w = luaL_checknumber(L, 4);
    nux_u32_t h = luaL_checknumber(L, 5);

    nux_canvas_rectangle(canvas, x, y, w, h);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_blit (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_t *texture
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));
    nux_b2i_t extent     = nux_lua_check_box2i(L, 3);
    nux_b2i_t tex_extent = nux_lua_check_box2i(L, 4);

    nux_canvas_blit(canvas, texture, extent, tex_extent);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_blit_sliced (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_t *texture
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));
    nux_b2i_t extent     = nux_lua_check_box2i(L, 3);
    nux_b2i_t tex_extent = nux_lua_check_box2i(L, 4);
    nux_b2i_t inner      = nux_lua_check_box2i(L, 5);

    nux_canvas_blit_sliced(canvas, texture, extent, tex_extent, inner);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_draw_line (lua_State *L)
{
    nux_v3_t a = nux_lua_check_vec3(L, 1);
    nux_v3_t b = nux_lua_check_vec3(L, 2);

    nux_graphics_draw_line(a, b);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_draw_dir (lua_State *L)
{
    nux_v3_t  origin = nux_lua_check_vec3(L, 1);
    nux_v3_t  dir    = nux_lua_check_vec3(L, 2);
    nux_f32_t length = luaL_checknumber(L, 3);

    nux_graphics_draw_dir(origin, dir, length);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_set_layer (lua_State *L)
{
    nux_u32_t layer = luaL_checknumber(L, 1);

    nux_graphics_set_layer(layer);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_set_color (lua_State *L)
{
    nux_u32_t color = luaL_checknumber(L, 1);

    nux_graphics_set_color(color);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_set_transform (lua_State *L)
{
    nux_m4_t transform = nux_lua_check_mat4(L, 1);

    nux_graphics_set_transform(transform);
    l_checkerror(L);

    return 0;
}
static int
l_camera_set_fov (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t fov = luaL_checknumber(L, 2);

    nux_camera_set_fov(e, fov);
    l_checkerror(L);

    return 0;
}
static int
l_camera_set_near (lua_State *L)
{
    nux_nid_t e    = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t near = luaL_checknumber(L, 2);

    nux_camera_set_near(e, near);
    l_checkerror(L);

    return 0;
}
static int
l_camera_set_far (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t far = luaL_checknumber(L, 2);

    nux_camera_set_far(e, far);
    l_checkerror(L);

    return 0;
}
static int
l_camera_set_aspect (lua_State *L)
{
    nux_nid_t e      = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t aspect = luaL_checknumber(L, 2);

    nux_camera_set_aspect(e, aspect);
    l_checkerror(L);

    return 0;
}
static int
l_camera_reset_aspect (lua_State *L)
{
    nux_nid_t       e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_viewport_t *viewport
        = nux_check_resource(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 2));

    nux_camera_reset_aspect(e, viewport);
    l_checkerror(L);

    return 0;
}
static int
l_camera_set_ortho (lua_State *L)
{
    nux_nid_t e     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_b32_t ortho = lua_toboolean(L, 2);

    nux_camera_set_ortho(e, ortho);
    l_checkerror(L);

    return 0;
}
static int
l_camera_set_ortho_size (lua_State *L)
{
    nux_nid_t e    = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v2_t  size = nux_lua_check_vec2(L, 2);

    nux_camera_set_ortho_size(e, size);
    l_checkerror(L);

    return 0;
}
static int
l_camera_get_projection (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_m4_t  ret = nux_camera_get_projection(e);
    l_checkerror(L);

    nux_lua_push_mat4(L, ret);
    return 1;
}
static int
l_camera_unproject (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v2_t  pos = nux_lua_check_vec2(L, 2);
    nux_v3_t  ret = nux_camera_unproject(e, pos);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_camera_set_render_mask (lua_State *L)
{
    nux_nid_t n    = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t mask = luaL_checknumber(L, 2);

    nux_camera_set_render_mask(n, mask);
    l_checkerror(L);

    return 0;
}
static int
l_camera_get_render_mask (lua_State *L)
{
    nux_nid_t n   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t ret = nux_camera_get_render_mask(n);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_staticmesh_set_mesh (lua_State *L)
{
    nux_nid_t   e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_mesh_t *mesh
        = nux_check_resource(NUX_RESOURCE_MESH, luaL_checkinteger(L, 2));

    nux_staticmesh_set_mesh(e, mesh);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_get_mesh (lua_State *L)
{
    nux_nid_t   n   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_mesh_t *ret = nux_staticmesh_get_mesh(n);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_staticmesh_set_texture (lua_State *L)
{
    nux_nid_t      e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_texture_t *texture
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));

    nux_staticmesh_set_texture(e, texture);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_get_texture (lua_State *L)
{
    nux_nid_t      n   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_texture_t *ret = nux_staticmesh_get_texture(n);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_staticmesh_set_colormap (lua_State *L)
{
    nux_nid_t      e = (nux_nid_t)luaL_checknumber(L, 1);
    nux_texture_t *colormap
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));

    nux_staticmesh_set_colormap(e, colormap);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_set_render_layer (lua_State *L)
{
    nux_nid_t n     = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t layer = luaL_checknumber(L, 2);

    nux_staticmesh_set_render_layer(n, layer);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_get_render_layer (lua_State *L)
{
    nux_nid_t n   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_u32_t ret = nux_staticmesh_get_render_layer(n);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_staticmesh_set_draw_bounds (lua_State *L)
{
    nux_nid_t n    = (nux_nid_t)luaL_checknumber(L, 1);
    nux_b32_t draw = lua_toboolean(L, 2);

    nux_staticmesh_set_draw_bounds(n, draw);
    l_checkerror(L);

    return 0;
}
static int
l_rigidbody_set_velocity (lua_State *L)
{
    nux_nid_t e        = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  velocity = nux_lua_check_vec3(L, 2);

    nux_rigidbody_set_velocity(e, velocity);
    l_checkerror(L);

    return 0;
}
static int
l_collider_set_sphere (lua_State *L)
{
    nux_nid_t e      = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t radius = luaL_checknumber(L, 2);

    nux_collider_set_sphere(e, radius);
    l_checkerror(L);

    return 0;
}
static int
l_collider_set_aabb (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  min = nux_lua_check_vec3(L, 2);
    nux_v3_t  max = nux_lua_check_vec3(L, 3);

    nux_collider_set_aabb(e, min, max);
    l_checkerror(L);

    return 0;
}
static int
l_physics_raycast (lua_State *L)
{
    nux_v3_t          pos = nux_lua_check_vec3(L, 1);
    nux_v3_t          dir = nux_lua_check_vec3(L, 2);
    nux_raycast_hit_t ret = nux_physics_raycast(pos, dir);
    l_checkerror(L);

    if (ret.node)
    {
        nux_lua_push_hit(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_physics_set_ground_height (lua_State *L)
{
    nux_f32_t height = luaL_checknumber(L, 1);

    nux_physics_set_ground_height(height);
    l_checkerror(L);

    return 0;
}
static int
l_stylesheet_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_stylesheet_t *ret = nux_stylesheet_new(arena);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_stylesheet_set (lua_State *L)
{
    nux_stylesheet_t *style
        = nux_check_resource(NUX_RESOURCE_STYLESHEET, luaL_checkinteger(L, 1));
    nux_stylesheet_property_t property = luaL_checknumber(L, 2);
    nux_texture_t            *texture
        = nux_check_resource(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 3));
    nux_b2i_t extent = nux_lua_check_box2i(L, 4);
    nux_b2i_t inner  = nux_lua_check_box2i(L, 5);

    nux_stylesheet_set(style, property, texture, extent, inner);
    l_checkerror(L);

    return 0;
}
static int
l_gui_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_check_resource(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_canvas_t *canvas
        = nux_check_resource(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 2));
    nux_gui_t *ret = nux_gui_new(arena, canvas);
    l_checkerror(L);

    nux_rid_t ret_rid = nux_resource_rid(ret);
    if (ret_rid)
    {
        lua_pushinteger(L, ret_rid);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_gui_push_style (lua_State *L)
{
    nux_gui_t *gui
        = nux_check_resource(NUX_RESOURCE_GUI, luaL_checkinteger(L, 1));
    nux_stylesheet_t *stylesheet
        = nux_check_resource(NUX_RESOURCE_STYLESHEET, luaL_checkinteger(L, 2));

    nux_gui_push_style(gui, stylesheet);
    l_checkerror(L);

    return 0;
}
static int
l_gui_pop_style (lua_State *L)
{
    nux_gui_t *gui
        = nux_check_resource(NUX_RESOURCE_GUI, luaL_checkinteger(L, 1));

    nux_gui_pop_style(gui);
    l_checkerror(L);

    return 0;
}
static int
l_gui_button (lua_State *L)
{
    nux_gui_t *gui
        = nux_check_resource(NUX_RESOURCE_GUI, luaL_checkinteger(L, 1));
    nux_u32_t x   = luaL_checknumber(L, 2);
    nux_u32_t y   = luaL_checknumber(L, 3);
    nux_u32_t w   = luaL_checknumber(L, 4);
    nux_u32_t h   = luaL_checknumber(L, 5);
    nux_b32_t ret = nux_gui_button(gui, x, y, w, h);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static const struct luaL_Reg lib_nux[]
    = { { "stat", l_stat },
        { "elapsed_time", l_elapsed_time },
        { "delta_time", l_delta_time },
        { "frame_index", l_frame_index },
        { "timestamp", l_timestamp },
        { "random", l_random },
        { "random01", l_random01 },
        { "new_arena", l_new_arena },
        { "clear_arena", l_clear_arena },
        { "arena_block_count", l_arena_block_count },
        { "arena_memory_usage", l_arena_memory_usage },
        { "set_log_level", l_set_log_level },
        { "reload_resource", l_reload_resource },
        { "module_requires", l_module_requires },
        { "event_type", l_event_type },
        { "process_event", l_process_event },
        { "io_cart_begin", l_io_cart_begin },
        { "io_write_cart_file", l_io_write_cart_file },
        { "mount_disk", l_mount_disk },
        { "file_exists", l_file_exists },
        { "open_file", l_open_file },
        { "close_file", l_close_file },
        { "seek_file", l_seek_file },
        { "file_size", l_file_size },
        { "new_inputmap", l_new_inputmap },
        { "bind_key", l_bind_key },
        { "bind_mouse_button", l_bind_mouse_button },
        { "bind_mouse_axis", l_bind_mouse_axis },
        { "set_input_map", l_set_input_map },
        { "is_pressed", l_is_pressed },
        { "is_released", l_is_released },
        { "is_just_pressed", l_is_just_pressed },
        { "is_just_released", l_is_just_released },
        { "axis", l_axis },
        { "cursor", l_cursor },
        { "wrap_cursor", l_wrap_cursor },
        { "lua_load", l_lua_load },
        { "transform_matrix", l_transform_matrix },
        { "transform_local_translation", l_transform_local_translation },
        { "transform_local_rotation", l_transform_local_rotation },
        { "transform_local_scale", l_transform_local_scale },
        { "transform_translation", l_transform_translation },
        { "transform_rotation", l_transform_rotation },
        { "transform_scale", l_transform_scale },
        { "transform_set_translation", l_transform_set_translation },
        { "transform_set_rotation", l_transform_set_rotation },
        { "transform_set_rotation_euler", l_transform_set_rotation_euler },
        { "transform_set_scale", l_transform_set_scale },
        { "transform_set_ortho", l_transform_set_ortho },
        { "transform_forward", l_transform_forward },
        { "transform_backward", l_transform_backward },
        { "transform_left", l_transform_left },
        { "transform_right", l_transform_right },
        { "transform_up", l_transform_up },
        { "transform_down", l_transform_down },
        { "transform_rotate", l_transform_rotate },
        { "transform_rotate_x", l_transform_rotate_x },
        { "transform_rotate_y", l_transform_rotate_y },
        { "transform_rotate_z", l_transform_rotate_z },
        { "transform_look_at", l_transform_look_at },
        { "query_new", l_query_new },
        { "query_new_any", l_query_new_any },
        { "query_includes", l_query_includes },
        { "query_excludes", l_query_excludes },
        { "query_next", l_query_next },
        { "scene_new", l_scene_new },
        { "scene_set_active", l_scene_set_active },
        { "node_create", l_node_create },
        { "node_delete", l_node_delete },
        { "node_valid", l_node_valid },
        { "node_parent", l_node_parent },
        { "node_set_parent", l_node_set_parent },
        { "node_sibling", l_node_sibling },
        { "node_child", l_node_child },
        { "node_add", l_node_add },
        { "node_remove", l_node_remove },
        { "node_has", l_node_has },
        { "node_instantiate", l_node_instantiate },
        { "scene_load_gltf", l_scene_load_gltf },
        { "viewport_new", l_viewport_new },
        { "viewport_set_mode", l_viewport_set_mode },
        { "viewport_set_extent", l_viewport_set_extent },
        { "viewport_set_anchor", l_viewport_set_anchor },
        { "viewport_set_layer", l_viewport_set_layer },
        { "viewport_set_clear_depth", l_viewport_set_clear_depth },
        { "viewport_set_auto_resize", l_viewport_set_auto_resize },
        { "viewport_set_camera", l_viewport_set_camera },
        { "viewport_set_texture", l_viewport_set_texture },
        { "viewport_get_normalized_viewport",
          l_viewport_get_normalized_viewport },
        { "viewport_to_global", l_viewport_to_global },
        { "viewport_to_local", l_viewport_to_local },
        { "texture_new", l_texture_new },
        { "texture_load", l_texture_load },
        { "texture_get_size", l_texture_get_size },
        { "palette_new", l_palette_new },
        { "palette_set_active", l_palette_set_active },
        { "palette_set_color", l_palette_set_color },
        { "palette_get_color", l_palette_get_color },
        { "mesh_new", l_mesh_new },
        { "mesh_new_cube", l_mesh_new_cube },
        { "mesh_new_plane", l_mesh_new_plane },
        { "mesh_update_bounds", l_mesh_update_bounds },
        { "mesh_bounds_min", l_mesh_bounds_min },
        { "mesh_bounds_max", l_mesh_bounds_max },
        { "mesh_set_origin", l_mesh_set_origin },
        { "mesh_transform", l_mesh_transform },
        { "mesh_size", l_mesh_size },
        { "canvas_new", l_canvas_new },
        { "canvas_get_texture", l_canvas_get_texture },
        { "canvas_get_size", l_canvas_get_size },
        { "canvas_set_clear_color", l_canvas_set_clear_color },
        { "canvas_set_wrap_mode", l_canvas_set_wrap_mode },
        { "canvas_text", l_canvas_text },
        { "canvas_rectangle", l_canvas_rectangle },
        { "canvas_blit", l_canvas_blit },
        { "canvas_blit_sliced", l_canvas_blit_sliced },
        { "graphics_draw_line", l_graphics_draw_line },
        { "graphics_draw_dir", l_graphics_draw_dir },
        { "graphics_set_layer", l_graphics_set_layer },
        { "graphics_set_color", l_graphics_set_color },
        { "graphics_set_transform", l_graphics_set_transform },
        { "camera_set_fov", l_camera_set_fov },
        { "camera_set_near", l_camera_set_near },
        { "camera_set_far", l_camera_set_far },
        { "camera_set_aspect", l_camera_set_aspect },
        { "camera_reset_aspect", l_camera_reset_aspect },
        { "camera_set_ortho", l_camera_set_ortho },
        { "camera_set_ortho_size", l_camera_set_ortho_size },
        { "camera_get_projection", l_camera_get_projection },
        { "camera_unproject", l_camera_unproject },
        { "camera_set_render_mask", l_camera_set_render_mask },
        { "camera_get_render_mask", l_camera_get_render_mask },
        { "staticmesh_set_mesh", l_staticmesh_set_mesh },
        { "staticmesh_get_mesh", l_staticmesh_get_mesh },
        { "staticmesh_set_texture", l_staticmesh_set_texture },
        { "staticmesh_get_texture", l_staticmesh_get_texture },
        { "staticmesh_set_colormap", l_staticmesh_set_colormap },
        { "staticmesh_set_render_layer", l_staticmesh_set_render_layer },
        { "staticmesh_get_render_layer", l_staticmesh_get_render_layer },
        { "staticmesh_set_draw_bounds", l_staticmesh_set_draw_bounds },
        { "rigidbody_set_velocity", l_rigidbody_set_velocity },
        { "collider_set_sphere", l_collider_set_sphere },
        { "collider_set_aabb", l_collider_set_aabb },
        { "physics_raycast", l_physics_raycast },
        { "physics_set_ground_height", l_physics_set_ground_height },
        { "stylesheet_new", l_stylesheet_new },
        { "stylesheet_set", l_stylesheet_set },
        { "gui_new", l_gui_new },
        { "gui_push_style", l_gui_push_style },
        { "gui_pop_style", l_gui_pop_style },
        { "gui_button", l_gui_button },
        { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_api (void)
{
    lua_State *L = nux_lua_state();
    lua_newtable(L);
    luaL_setfuncs(L, lib_nux, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "ERROR_NONE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ERROR_OUT_OF_MEMORY");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "ERROR_INVALID_TEXTURE_SIZE");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "ERROR_WASM_RUNTIME");
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "ERROR_CART_EOF");
    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "ERROR_CART_MOUNT");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "STAT_FPS");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "STAT_SCREEN_WIDTH");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "STAT_SCREEN_HEIGHT");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "STAT_TIMESTAMP");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "STAT_MAX");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "LOG_DEBUG");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "LOG_INFO");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "LOG_WARNING");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LOG_ERROR");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "RESOURCE_NULL");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "RESOURCE_ANY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "RESOURCE_ARENA");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "RESOURCE_LUA_MODULE");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "RESOURCE_TEXTURE");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "RESOURCE_MESH");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "RESOURCE_PALETTE");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "RESOURCE_VIEWPORT");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "RESOURCE_CANVAS");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "RESOURCE_FONT");
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "RESOURCE_FILE");
    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "RESOURCE_DISK");
    lua_pushinteger(L, 12);
    lua_setfield(L, -2, "RESOURCE_SCENE");
    lua_pushinteger(L, 13);
    lua_setfield(L, -2, "RESOURCE_QUERY");
    lua_pushinteger(L, 14);
    lua_setfield(L, -2, "RESOURCE_EVENT");
    lua_pushinteger(L, 15);
    lua_setfield(L, -2, "RESOURCE_INPUTMAP");
    lua_pushinteger(L, 16);
    lua_setfield(L, -2, "RESOURCE_GUI");
    lua_pushinteger(L, 17);
    lua_setfield(L, -2, "RESOURCE_STYLESHEET");
    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "RESOURCE_MAX");
    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "MODULE_NO_DATA_INITIALIZATION");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "EVENT_LUA");
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "NAME_MAX");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "SYSTEM_PRE_UPDATE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "SYSTEM_UPDATE");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "SYSTEM_POST_UPDATE");
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "FILE_MAX");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "DISK_MAX");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "IO_READ");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "IO_READ_WRITE");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "INPUT_UNMAPPED");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "INPUT_KEY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "INPUT_MOUSE_BUTTON");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "INPUT_MOUSE_AXIS");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "INPUT_GAMEPAD_BUTTON");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "INPUT_GAMEPAD_AXIS");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "BUTTON_PRESSED");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "BUTTON_RELEASED");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "KEY_SPACE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "KEY_APOSTROPHE");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "KEY_COMMA");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "KEY_MINUS");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "KEY_PERIOD");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "KEY_SLASH");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "KEY_NUM0");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "KEY_NUM1");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "KEY_NUM2");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "KEY_NUM3");
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "KEY_NUM4");
    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "KEY_NUM5");
    lua_pushinteger(L, 12);
    lua_setfield(L, -2, "KEY_NUM6");
    lua_pushinteger(L, 13);
    lua_setfield(L, -2, "KEY_NUM7");
    lua_pushinteger(L, 14);
    lua_setfield(L, -2, "KEY_NUM8");
    lua_pushinteger(L, 15);
    lua_setfield(L, -2, "KEY_NUM9");
    lua_pushinteger(L, 16);
    lua_setfield(L, -2, "KEY_SEMICOLON");
    lua_pushinteger(L, 17);
    lua_setfield(L, -2, "KEY_EQUAL");
    lua_pushinteger(L, 18);
    lua_setfield(L, -2, "KEY_A");
    lua_pushinteger(L, 19);
    lua_setfield(L, -2, "KEY_B");
    lua_pushinteger(L, 20);
    lua_setfield(L, -2, "KEY_C");
    lua_pushinteger(L, 21);
    lua_setfield(L, -2, "KEY_D");
    lua_pushinteger(L, 22);
    lua_setfield(L, -2, "KEY_E");
    lua_pushinteger(L, 23);
    lua_setfield(L, -2, "KEY_F");
    lua_pushinteger(L, 24);
    lua_setfield(L, -2, "KEY_G");
    lua_pushinteger(L, 25);
    lua_setfield(L, -2, "KEY_H");
    lua_pushinteger(L, 26);
    lua_setfield(L, -2, "KEY_I");
    lua_pushinteger(L, 27);
    lua_setfield(L, -2, "KEY_J");
    lua_pushinteger(L, 29);
    lua_setfield(L, -2, "KEY_K");
    lua_pushinteger(L, 30);
    lua_setfield(L, -2, "KEY_L");
    lua_pushinteger(L, 31);
    lua_setfield(L, -2, "KEY_M");
    lua_pushinteger(L, 32);
    lua_setfield(L, -2, "KEY_N");
    lua_pushinteger(L, 33);
    lua_setfield(L, -2, "KEY_O");
    lua_pushinteger(L, 34);
    lua_setfield(L, -2, "KEY_P");
    lua_pushinteger(L, 35);
    lua_setfield(L, -2, "KEY_Q");
    lua_pushinteger(L, 36);
    lua_setfield(L, -2, "KEY_R");
    lua_pushinteger(L, 37);
    lua_setfield(L, -2, "KEY_S");
    lua_pushinteger(L, 38);
    lua_setfield(L, -2, "KEY_T");
    lua_pushinteger(L, 39);
    lua_setfield(L, -2, "KEY_U");
    lua_pushinteger(L, 40);
    lua_setfield(L, -2, "KEY_V");
    lua_pushinteger(L, 41);
    lua_setfield(L, -2, "KEY_W");
    lua_pushinteger(L, 42);
    lua_setfield(L, -2, "KEY_X");
    lua_pushinteger(L, 43);
    lua_setfield(L, -2, "KEY_Y");
    lua_pushinteger(L, 44);
    lua_setfield(L, -2, "KEY_Z");
    lua_pushinteger(L, 45);
    lua_setfield(L, -2, "KEY_LEFT_BRACKET");
    lua_pushinteger(L, 46);
    lua_setfield(L, -2, "KEY_BACKSLASH");
    lua_pushinteger(L, 47);
    lua_setfield(L, -2, "KEY_RIGHT_BRACKET");
    lua_pushinteger(L, 48);
    lua_setfield(L, -2, "KEY_GRAVE_ACCENT");
    lua_pushinteger(L, 49);
    lua_setfield(L, -2, "KEY_ESCAPE");
    lua_pushinteger(L, 50);
    lua_setfield(L, -2, "KEY_ENTER");
    lua_pushinteger(L, 51);
    lua_setfield(L, -2, "KEY_TAB");
    lua_pushinteger(L, 52);
    lua_setfield(L, -2, "KEY_BACKSPACE");
    lua_pushinteger(L, 53);
    lua_setfield(L, -2, "KEY_INSERT");
    lua_pushinteger(L, 54);
    lua_setfield(L, -2, "KEY_DELETE");
    lua_pushinteger(L, 55);
    lua_setfield(L, -2, "KEY_RIGHT");
    lua_pushinteger(L, 56);
    lua_setfield(L, -2, "KEY_LEFT");
    lua_pushinteger(L, 57);
    lua_setfield(L, -2, "KEY_DOWN");
    lua_pushinteger(L, 58);
    lua_setfield(L, -2, "KEY_UP");
    lua_pushinteger(L, 59);
    lua_setfield(L, -2, "KEY_PAGE_UP");
    lua_pushinteger(L, 60);
    lua_setfield(L, -2, "KEY_PAGE_DOWN");
    lua_pushinteger(L, 61);
    lua_setfield(L, -2, "KEY_HOME");
    lua_pushinteger(L, 62);
    lua_setfield(L, -2, "KEY_END");
    lua_pushinteger(L, 63);
    lua_setfield(L, -2, "KEY_CAPS_LOCK");
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "KEY_SCROLL_LOCK");
    lua_pushinteger(L, 65);
    lua_setfield(L, -2, "KEY_NUM_LOCK");
    lua_pushinteger(L, 66);
    lua_setfield(L, -2, "KEY_PRINT_SCREEN");
    lua_pushinteger(L, 67);
    lua_setfield(L, -2, "KEY_PAUSE");
    lua_pushinteger(L, 68);
    lua_setfield(L, -2, "KEY_F1");
    lua_pushinteger(L, 69);
    lua_setfield(L, -2, "KEY_F2");
    lua_pushinteger(L, 70);
    lua_setfield(L, -2, "KEY_F3");
    lua_pushinteger(L, 71);
    lua_setfield(L, -2, "KEY_F4");
    lua_pushinteger(L, 72);
    lua_setfield(L, -2, "KEY_F5");
    lua_pushinteger(L, 73);
    lua_setfield(L, -2, "KEY_F6");
    lua_pushinteger(L, 74);
    lua_setfield(L, -2, "KEY_F7");
    lua_pushinteger(L, 75);
    lua_setfield(L, -2, "KEY_F8");
    lua_pushinteger(L, 76);
    lua_setfield(L, -2, "KEY_F9");
    lua_pushinteger(L, 77);
    lua_setfield(L, -2, "KEY_F10");
    lua_pushinteger(L, 78);
    lua_setfield(L, -2, "KEY_F11");
    lua_pushinteger(L, 79);
    lua_setfield(L, -2, "KEY_F12");
    lua_pushinteger(L, 80);
    lua_setfield(L, -2, "KEY_F13");
    lua_pushinteger(L, 81);
    lua_setfield(L, -2, "KEY_F14");
    lua_pushinteger(L, 82);
    lua_setfield(L, -2, "KEY_F15");
    lua_pushinteger(L, 83);
    lua_setfield(L, -2, "KEY_F16");
    lua_pushinteger(L, 84);
    lua_setfield(L, -2, "KEY_F17");
    lua_pushinteger(L, 85);
    lua_setfield(L, -2, "KEY_F18");
    lua_pushinteger(L, 86);
    lua_setfield(L, -2, "KEY_F19");
    lua_pushinteger(L, 87);
    lua_setfield(L, -2, "KEY_F20");
    lua_pushinteger(L, 88);
    lua_setfield(L, -2, "KEY_F21");
    lua_pushinteger(L, 89);
    lua_setfield(L, -2, "KEY_F22");
    lua_pushinteger(L, 90);
    lua_setfield(L, -2, "KEY_F23");
    lua_pushinteger(L, 91);
    lua_setfield(L, -2, "KEY_F24");
    lua_pushinteger(L, 92);
    lua_setfield(L, -2, "KEY_F25");
    lua_pushinteger(L, 93);
    lua_setfield(L, -2, "KEY_KP_0");
    lua_pushinteger(L, 94);
    lua_setfield(L, -2, "KEY_KP_1");
    lua_pushinteger(L, 95);
    lua_setfield(L, -2, "KEY_KP_2");
    lua_pushinteger(L, 96);
    lua_setfield(L, -2, "KEY_KP_3");
    lua_pushinteger(L, 97);
    lua_setfield(L, -2, "KEY_KP_4");
    lua_pushinteger(L, 98);
    lua_setfield(L, -2, "KEY_KP_5");
    lua_pushinteger(L, 99);
    lua_setfield(L, -2, "KEY_KP_6");
    lua_pushinteger(L, 100);
    lua_setfield(L, -2, "KEY_KP_7");
    lua_pushinteger(L, 101);
    lua_setfield(L, -2, "KEY_KP_8");
    lua_pushinteger(L, 102);
    lua_setfield(L, -2, "KEY_KP_9");
    lua_pushinteger(L, 103);
    lua_setfield(L, -2, "KEY_KP_DECIMAL");
    lua_pushinteger(L, 104);
    lua_setfield(L, -2, "KEY_KP_DIVIDE");
    lua_pushinteger(L, 105);
    lua_setfield(L, -2, "KEY_KP_MULTIPLY");
    lua_pushinteger(L, 106);
    lua_setfield(L, -2, "KEY_KP_SUBTRACT");
    lua_pushinteger(L, 107);
    lua_setfield(L, -2, "KEY_KP_ADD");
    lua_pushinteger(L, 108);
    lua_setfield(L, -2, "KEY_KP_ENTER");
    lua_pushinteger(L, 109);
    lua_setfield(L, -2, "KEY_KP_EQUAL");
    lua_pushinteger(L, 110);
    lua_setfield(L, -2, "KEY_LEFT_SHIFT");
    lua_pushinteger(L, 111);
    lua_setfield(L, -2, "KEY_LEFT_CONTROL");
    lua_pushinteger(L, 112);
    lua_setfield(L, -2, "KEY_LEFT_ALT");
    lua_pushinteger(L, 113);
    lua_setfield(L, -2, "KEY_LEFT_SUPER");
    lua_pushinteger(L, 114);
    lua_setfield(L, -2, "KEY_RIGHT_SHIFT");
    lua_pushinteger(L, 115);
    lua_setfield(L, -2, "KEY_RIGHT_CONTROL");
    lua_pushinteger(L, 116);
    lua_setfield(L, -2, "KEY_RIGHT_ALT");
    lua_pushinteger(L, 117);
    lua_setfield(L, -2, "KEY_RIGHT_SUPER");
    lua_pushinteger(L, 118);
    lua_setfield(L, -2, "KEY_MENU");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "MOUSE_BUTTON_LEFT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "MOUSE_BUTTON_RIGHT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "MOUSE_BUTTON_MIDDLE");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "MOUSE_WHEEL_UP");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "MOUSE_WHEEL_DOWN");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "MOUSE_MOTION_RIGHT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "MOUSE_MOTION_LEFT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "MOUSE_MOTION_DOWN");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "MOUSE_MOTION_UP");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "MOUSE_SCROLL_UP");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "MOUSE_SCROLL_DOWN");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "GAMEPAD_A");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "GAMEPAD_X");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "GAMEPAD_Y");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "GAMEPAD_B");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "GAMEPAD_DPAD_UP");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "GAMEPAD_DPAD_DOWN");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "GAMEPAD_DPAD_LEFT");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "GAMEPAD_DPAD_RIGHT");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "GAMEPAD_SHOULDER_LEFT");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "GAMEPAD_SHOULDER_RIGHT");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "GAMEPAD_LSTICK_LEFT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "GAMEPAD_LSTICK_RIGHT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "GAMEPAD_LSTICK_UP");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "GAMEPAD_LSTICK_DOWN");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "GAMEPAD_RSTICK_LEFT");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "GAMEPAD_RSTICK_RIGHT");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "GAMEPAD_RSTICK_UP");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "GAMEPAD_RSTICK_DOWN");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "GAMEPAD_LTRIGGER");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "GAMEPAD_RTRIGGER");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "CURSOR_UP");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "CURSOR_DOWN");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "CURSOR_LEFT");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "CURSOR_RIGHT");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "CONTROLLER_MAX");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "COMPONENT_TRANSFORM");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "COMPONENT_CAMERA");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "COMPONENT_STATICMESH");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "COMPONENT_RIGIDBODY");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "COMPONENT_COLLIDER");
    lua_pushinteger(L, 16);
    lua_setfield(L, -2, "COMPONENT_MAX");
    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "PALETTE_SIZE");
    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "COLORMAP_SIZE");
    lua_pushinteger(L, 0x1);
    lua_setfield(L, -2, "LAYER_DEFAULT");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "VERTEX_TRIANGLES");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "VERTEX_LINES");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "VERTEX_POINTS");
    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "VERTEX_POSITION");
    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "VERTEX_TEXCOORD");
    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "VERTEX_COLOR");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TEXTURE_IMAGE_RGBA");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "TEXTURE_IMAGE_INDEX");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "TEXTURE_RENDER_TARGET");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TEXTURE_WRAP_CLAMP");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "TEXTURE_WRAP_REPEAT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "TEXTURE_WRAP_MIRROR");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "COLOR_TRANSPARENT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "COLOR_WHITE");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "COLOR_RED");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "COLOR_GREEN");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "COLOR_BLUE");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "COLOR_BACKGROUND");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "VIEWPORT_HIDDEN");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "VIEWPORT_FIXED");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "VIEWPORT_FIXED_BEST_FIT");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "VIEWPORT_STRETCH_KEEP_ASPECT");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "VIEWPORT_STRETCH");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "ANCHOR_CENTER");
    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "ANCHOR_TOP");
    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "ANCHOR_BOTTOM");
    lua_pushinteger(L, 1 << 3);
    lua_setfield(L, -2, "ANCHOR_LEFT");
    lua_pushinteger(L, 1 << 4);
    lua_setfield(L, -2, "ANCHOR_RIGHT");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "COLLIDER_SPHERE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "COLLIDER_AABB");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "CONTROLLER_MODE_SELECTION");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "CONTROLLER_MODE_CURSOR");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "STYLESHEET_BUTTON_PRESSED");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "STYLESHEET_BUTTON_RELEASED");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "STYLESHEET_BUTTON_HOVERED");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "STYLESHEET_CHECKBOX_CHECKED");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "STYLESHEET_CHECKBOX_UNCHECKED");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "STYLESHEET_CURSOR");
    lua_setglobal(L, "nux");
    return NUX_SUCCESS;
}