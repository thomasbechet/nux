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
l_stat_get (lua_State *L)
{
    nux_stat_t info = luaL_checknumber(L, 1);
    nux_u32_t  ret  = nux_stat_get(info);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_time_elapsed (lua_State *L)
{
    nux_f32_t ret = nux_time_elapsed();
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_time_delta (lua_State *L)
{
    nux_f32_t ret = nux_time_delta();
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_time_frame (lua_State *L)
{
    nux_u32_t ret = nux_time_frame();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_time_epoch (lua_State *L)
{
    nux_u64_t ret = nux_time_epoch();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_random_next (lua_State *L)
{
    nux_u32_t ret = nux_random_next();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_random_nextf (lua_State *L)
{
    nux_f32_t ret = nux_random_nextf();
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_arena_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_arena_t *ret = nux_arena_new(arena);
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
l_arena_clear (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));

    nux_arena_clear(arena);
    l_checkerror(L);

    return 0;
}
static int
l_arena_block_count (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_arena_block_count(arena);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_arena_memory_usage (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_arena_memory_usage(arena);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_logger_set_level (lua_State *L)
{
    nux_log_level_t level = luaL_checknumber(L, 1);

    nux_logger_set_level(level);
    l_checkerror(L);

    return 0;
}
static int
l_resource_reload (lua_State *L)
{
    nux_rid_t    rid = (nux_rid_t)luaL_checknumber(L, 1);
    nux_status_t ret = nux_resource_reload(rid);
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
        = nux_resource_check(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 1));
    nux_event_type_t ret = nux_event_type(event);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_event_process (lua_State *L)
{
    nux_event_t *event
        = nux_resource_check(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 1));

    nux_event_process(event);
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
l_file_open (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t   *path = luaL_checkstring(L, 2);
    nux_io_mode_t     mode = luaL_checknumber(L, 3);
    const nux_file_t *ret  = nux_file_open(arena, path, mode);
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
l_file_close (lua_State *L)
{
    nux_file_t *file
        = nux_resource_check(NUX_RESOURCE_FILE, luaL_checkinteger(L, 1));

    nux_file_close(file);
    l_checkerror(L);

    return 0;
}
static int
l_file_seek (lua_State *L)
{
    nux_file_t *file
        = nux_resource_check(NUX_RESOURCE_FILE, luaL_checkinteger(L, 1));
    nux_u32_t    cursor = luaL_checknumber(L, 2);
    nux_status_t ret    = nux_file_seek(file, cursor);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_file_size (lua_State *L)
{
    nux_file_t *file
        = nux_resource_check(NUX_RESOURCE_FILE, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_file_size(file);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_inputmap_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_inputmap_t *ret = nux_inputmap_new(arena);
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
l_inputmap_bind_key (lua_State *L)
{
    nux_inputmap_t *map
        = nux_resource_check(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 1));
    const nux_c8_t *name = luaL_checkstring(L, 2);
    nux_key_t       key  = luaL_checknumber(L, 3);

    nux_inputmap_bind_key(map, name, key);
    l_checkerror(L);

    return 0;
}
static int
l_inputmap_bind_mouse_button (lua_State *L)
{
    nux_inputmap_t *map
        = nux_resource_check(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 1));
    const nux_c8_t    *name   = luaL_checkstring(L, 2);
    nux_mouse_button_t button = luaL_checknumber(L, 3);

    nux_inputmap_bind_mouse_button(map, name, button);
    l_checkerror(L);

    return 0;
}
static int
l_inputmap_bind_mouse_axis (lua_State *L)
{
    nux_inputmap_t *map
        = nux_resource_check(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 1));
    const nux_c8_t  *name      = luaL_checkstring(L, 2);
    nux_mouse_axis_t axis      = luaL_checknumber(L, 3);
    nux_f32_t        sensivity = luaL_checknumber(L, 4);

    nux_inputmap_bind_mouse_axis(map, name, axis, sensivity);
    l_checkerror(L);

    return 0;
}
static int
l_input_set_inputmap (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    nux_inputmap_t *map
        = nux_resource_check(NUX_RESOURCE_INPUTMAP, luaL_checkinteger(L, 2));

    nux_input_set_inputmap(controller, map);
    l_checkerror(L);

    return 0;
}
static int
l_input_pressed (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_input_pressed(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_input_released (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_input_released(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_input_just_pressed (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_input_just_pressed(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_input_just_released (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_b32_t       ret        = nux_input_just_released(controller, name);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_input_value (lua_State *L)
{
    nux_u32_t       controller = luaL_checknumber(L, 1);
    const nux_c8_t *name       = luaL_checkstring(L, 2);
    nux_f32_t       ret        = nux_input_value(controller, name);
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_input_cursor (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_v2_t  ret        = nux_input_cursor(controller);
    l_checkerror(L);

    nux_lua_push_vec2(L, ret);
    return 1;
}
static int
l_input_wrap_cursor (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_f32_t x          = luaL_checknumber(L, 2);
    nux_f32_t y          = luaL_checknumber(L, 3);

    nux_input_wrap_cursor(controller, x, y);
    l_checkerror(L);

    return 0;
}
static int
l_lua_load (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t  *path = luaL_checkstring(L, 2);
    const nux_lua_t *ret  = nux_lua_load(arena, path);
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t          include_count = luaL_checknumber(L, 2);
    nux_u32_t          exclude_count = luaL_checknumber(L, 3);
    const nux_query_t *ret = nux_query_new(arena, include_count, exclude_count);
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_query_t *ret = nux_query_new_any(arena);
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
        = nux_resource_check(NUX_RESOURCE_QUERY, luaL_checkinteger(L, 1));
    nux_u32_t c = luaL_checknumber(L, 2);

    nux_query_includes(it, c);
    l_checkerror(L);

    return 0;
}
static int
l_query_excludes (lua_State *L)
{
    nux_query_t *it
        = nux_resource_check(NUX_RESOURCE_QUERY, luaL_checkinteger(L, 1));
    nux_u32_t c = luaL_checknumber(L, 2);

    nux_query_excludes(it, c);
    l_checkerror(L);

    return 0;
}
static int
l_query_next (lua_State *L)
{
    nux_query_t *it
        = nux_resource_check(NUX_RESOURCE_QUERY, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_scene_t *ret = nux_scene_new(arena);
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
        = nux_resource_check(NUX_RESOURCE_SCENE, luaL_checkinteger(L, 1));
    nux_status_t ret = nux_scene_set_active(scene);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_scene_load_gltf (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t    *path = luaL_checkstring(L, 2);
    const nux_scene_t *ret  = nux_scene_load_gltf(arena, path);
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
        = nux_resource_check(NUX_RESOURCE_SCENE, luaL_checkinteger(L, 1));
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
l_viewport_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_texture_t *target
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));
    const nux_viewport_t *ret = nux_viewport_new(arena, target);
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
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_viewport_mode_t mode = luaL_checknumber(L, 2);

    nux_viewport_set_mode(vp, mode);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_extent (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_b2i_t extent = nux_lua_check_box2i(L, 2);

    nux_viewport_set_extent(vp, extent);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_anchor (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_u32_t anchor = luaL_checknumber(L, 2);

    nux_viewport_set_anchor(vp, anchor);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_layer (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_i32_t layer = luaL_checknumber(L, 2);

    nux_viewport_set_layer(vp, layer);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_clear_depth (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_b32_t clear = lua_toboolean(L, 2);

    nux_viewport_set_clear_depth(vp, clear);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_auto_resize (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_b32_t enable = lua_toboolean(L, 2);

    nux_viewport_set_auto_resize(vp, enable);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_camera (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_nid_t camera = (nux_nid_t)luaL_checknumber(L, 2);

    nux_viewport_set_camera(vp, camera);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_set_texture (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_texture_t *texture
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));

    nux_viewport_set_texture(vp, texture);
    l_checkerror(L);

    return 0;
}
static int
l_viewport_get_normalized_viewport (lua_State *L)
{
    nux_viewport_t *viewport
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_v4_t ret = nux_viewport_get_normalized_viewport(viewport);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
    return 1;
}
static int
l_viewport_to_global (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_texture_type_t   format = luaL_checknumber(L, 2);
    nux_u32_t            w      = luaL_checknumber(L, 3);
    nux_u32_t            h      = luaL_checknumber(L, 4);
    const nux_texture_t *ret    = nux_texture_new(arena, format, w, h);
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_c8_t      *path = luaL_checkstring(L, 2);
    const nux_texture_t *ret  = nux_texture_load(arena, path);
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
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 1));
    nux_v2i_t ret = nux_texture_get_size(texture);
    l_checkerror(L);

    nux_lua_push_vec2i(L, ret);
    return 1;
}
static int
l_palette_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t            size = luaL_checknumber(L, 2);
    const nux_palette_t *ret  = nux_palette_new(arena, size);
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
        = nux_resource_check(NUX_RESOURCE_PALETTE, luaL_checkinteger(L, 1));

    nux_palette_set_active(palette);
    l_checkerror(L);

    return 0;
}
static int
l_palette_set_color (lua_State *L)
{
    nux_palette_t *palette
        = nux_resource_check(NUX_RESOURCE_PALETTE, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_PALETTE, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t              capa       = luaL_checknumber(L, 2);
    nux_vertex_attribute_t attributes = luaL_checknumber(L, 3);
    nux_vertex_primitive_t primitive  = luaL_checknumber(L, 4);
    const nux_mesh_t *ret = nux_mesh_new(arena, capa, attributes, primitive);
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_f32_t         sx  = luaL_checknumber(L, 2);
    nux_f32_t         sy  = luaL_checknumber(L, 3);
    nux_f32_t         sz  = luaL_checknumber(L, 4);
    const nux_mesh_t *ret = nux_mesh_new_cube(arena, sx, sy, sz);
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_f32_t         sx  = luaL_checknumber(L, 2);
    nux_f32_t         sy  = luaL_checknumber(L, 3);
    const nux_mesh_t *ret = nux_mesh_new_plane(arena, sx, sy);
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
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));

    nux_mesh_update_bounds(mesh);
    l_checkerror(L);

    return 0;
}
static int
l_mesh_bounds_min (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_v3_t ret = nux_mesh_bounds_min(mesh);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_mesh_bounds_max (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_v3_t ret = nux_mesh_bounds_max(mesh);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_mesh_set_origin (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_v3_t origin = nux_lua_check_vec3(L, 2);

    nux_mesh_set_origin(mesh, origin);
    l_checkerror(L);

    return 0;
}
static int
l_mesh_transform (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_m4_t transform = nux_lua_check_mat4(L, 2);

    nux_mesh_transform(mesh, transform);
    l_checkerror(L);

    return 0;
}
static int
l_mesh_size (lua_State *L)
{
    nux_mesh_t *mesh
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_mesh_size(mesh);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_canvas_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t           width  = luaL_checknumber(L, 2);
    nux_u32_t           height = luaL_checknumber(L, 3);
    const nux_canvas_t *ret    = nux_canvas_new(arena, width, height);
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
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    const nux_texture_t *ret = nux_canvas_get_texture(canvas);
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
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_v2i_t ret = nux_canvas_get_size(canvas);
    l_checkerror(L);

    nux_lua_push_vec2i(L, ret);
    return 1;
}
static int
l_canvas_set_clear_color (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_u32_t color = luaL_checknumber(L, 2);

    nux_canvas_set_clear_color(canvas, color);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_set_wrap_mode (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_wrap_mode_t mode = luaL_checknumber(L, 2);

    nux_canvas_set_wrap_mode(canvas, mode);
    l_checkerror(L);

    return 0;
}
static int
l_canvas_text (lua_State *L)
{
    nux_canvas_t *canvas
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
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
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_t *texture
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));
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
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 1));
    nux_texture_t *texture
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));
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
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 2));

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
        = nux_resource_check(NUX_RESOURCE_MESH, luaL_checkinteger(L, 2));

    nux_staticmesh_set_mesh(e, mesh);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_get_mesh (lua_State *L)
{
    nux_nid_t         n   = (nux_nid_t)luaL_checknumber(L, 1);
    const nux_mesh_t *ret = nux_staticmesh_get_mesh(n);
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
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));

    nux_staticmesh_set_texture(e, texture);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_get_texture (lua_State *L)
{
    nux_nid_t            n   = (nux_nid_t)luaL_checknumber(L, 1);
    const nux_texture_t *ret = nux_staticmesh_get_texture(n);
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
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 2));

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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    const nux_stylesheet_t *ret = nux_stylesheet_new(arena);
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
        = nux_resource_check(NUX_RESOURCE_STYLESHEET, luaL_checkinteger(L, 1));
    nux_stylesheet_property_t property = luaL_checknumber(L, 2);
    nux_texture_t            *texture
        = nux_resource_check(NUX_RESOURCE_TEXTURE, luaL_checkinteger(L, 3));
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
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_canvas_t *canvas
        = nux_resource_check(NUX_RESOURCE_CANVAS, luaL_checkinteger(L, 2));
    const nux_gui_t *ret = nux_gui_new(arena, canvas);
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
        = nux_resource_check(NUX_RESOURCE_GUI, luaL_checkinteger(L, 1));
    nux_stylesheet_t *stylesheet
        = nux_resource_check(NUX_RESOURCE_STYLESHEET, luaL_checkinteger(L, 2));

    nux_gui_push_style(gui, stylesheet);
    l_checkerror(L);

    return 0;
}
static int
l_gui_pop_style (lua_State *L)
{
    nux_gui_t *gui
        = nux_resource_check(NUX_RESOURCE_GUI, luaL_checkinteger(L, 1));

    nux_gui_pop_style(gui);
    l_checkerror(L);

    return 0;
}
static int
l_gui_button (lua_State *L)
{
    nux_gui_t *gui
        = nux_resource_check(NUX_RESOURCE_GUI, luaL_checkinteger(L, 1));
    nux_u32_t x   = luaL_checknumber(L, 2);
    nux_u32_t y   = luaL_checknumber(L, 3);
    nux_u32_t w   = luaL_checknumber(L, 4);
    nux_u32_t h   = luaL_checknumber(L, 5);
    nux_b32_t ret = nux_gui_button(gui, x, y, w, h);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static const struct luaL_Reg lib_stat[]
    = { { "get", l_stat_get }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_time[]   = { { "elapsed", l_time_elapsed },
                                              { "delta", l_time_delta },
                                              { "frame", l_time_frame },
                                              { "epoch", l_time_epoch },
                                              { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_random[] = { { "next", l_random_next },
                                              { "nextf", l_random_nextf },
                                              { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_arena[]
    = { { "new", l_arena_new },
        { "clear", l_arena_clear },
        { "block_count", l_arena_block_count },
        { "memory_usage", l_arena_memory_usage },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_logger[]
    = { { "set_level", l_logger_set_level }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_resource[]
    = { { "reload", l_resource_reload }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_module[]
    = { { "requires", l_module_requires }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_event[] = { { "type", l_event_type },
                                             { "process", l_event_process },
                                             { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_io[]
    = { { "cart_begin", l_io_cart_begin },
        { "write_cart_file", l_io_write_cart_file },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_mount[]
    = { { "disk", l_mount_disk }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_file[]
    = { { "exists", l_file_exists }, { "open", l_file_open },
        { "close", l_file_close },   { "seek", l_file_seek },
        { "size", l_file_size },     { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_error[]  = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_log[]    = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_name[]   = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_system[] = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_disk[]   = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_inputmap[]
    = { { "new", l_inputmap_new },
        { "bind_key", l_inputmap_bind_key },
        { "bind_mouse_button", l_inputmap_bind_mouse_button },
        { "bind_mouse_axis", l_inputmap_bind_mouse_axis },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_input[]
    = { { "set_inputmap", l_input_set_inputmap },
        { "pressed", l_input_pressed },
        { "released", l_input_released },
        { "just_pressed", l_input_just_pressed },
        { "just_released", l_input_just_released },
        { "value", l_input_value },
        { "cursor", l_input_cursor },
        { "wrap_cursor", l_input_wrap_cursor },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_button[]     = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_key[]        = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_mouse[]      = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_gamepad[]    = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_cursor[]     = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_controller[] = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_lua[]
    = { { "load", l_lua_load }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_component[] = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_transform[]
    = { { "matrix", l_transform_matrix },
        { "local_translation", l_transform_local_translation },
        { "local_rotation", l_transform_local_rotation },
        { "local_scale", l_transform_local_scale },
        { "translation", l_transform_translation },
        { "rotation", l_transform_rotation },
        { "scale", l_transform_scale },
        { "set_translation", l_transform_set_translation },
        { "set_rotation", l_transform_set_rotation },
        { "set_rotation_euler", l_transform_set_rotation_euler },
        { "set_scale", l_transform_set_scale },
        { "set_ortho", l_transform_set_ortho },
        { "forward", l_transform_forward },
        { "backward", l_transform_backward },
        { "left", l_transform_left },
        { "right", l_transform_right },
        { "up", l_transform_up },
        { "down", l_transform_down },
        { "rotate", l_transform_rotate },
        { "rotate_x", l_transform_rotate_x },
        { "rotate_y", l_transform_rotate_y },
        { "rotate_z", l_transform_rotate_z },
        { "look_at", l_transform_look_at },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_query[]
    = { { "new", l_query_new },           { "new_any", l_query_new_any },
        { "includes", l_query_includes }, { "excludes", l_query_excludes },
        { "next", l_query_next },         { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_scene[]
    = { { "new", l_scene_new },
        { "set_active", l_scene_set_active },
        { "load_gltf", l_scene_load_gltf },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_node[]
    = { { "create", l_node_create },
        { "delete", l_node_delete },
        { "valid", l_node_valid },
        { "parent", l_node_parent },
        { "set_parent", l_node_set_parent },
        { "sibling", l_node_sibling },
        { "child", l_node_child },
        { "add", l_node_add },
        { "remove", l_node_remove },
        { "has", l_node_has },
        { "instantiate", l_node_instantiate },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_viewport[]
    = { { "new", l_viewport_new },
        { "set_mode", l_viewport_set_mode },
        { "set_extent", l_viewport_set_extent },
        { "set_anchor", l_viewport_set_anchor },
        { "set_layer", l_viewport_set_layer },
        { "set_clear_depth", l_viewport_set_clear_depth },
        { "set_auto_resize", l_viewport_set_auto_resize },
        { "set_camera", l_viewport_set_camera },
        { "set_texture", l_viewport_set_texture },
        { "get_normalized_viewport", l_viewport_get_normalized_viewport },
        { "to_global", l_viewport_to_global },
        { "to_local", l_viewport_to_local },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_texture[]
    = { { "new", l_texture_new },
        { "load", l_texture_load },
        { "get_size", l_texture_get_size },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_palette[]
    = { { "new", l_palette_new },
        { "set_active", l_palette_set_active },
        { "set_color", l_palette_set_color },
        { "get_color", l_palette_get_color },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_mesh[]
    = { { "new", l_mesh_new },
        { "new_cube", l_mesh_new_cube },
        { "new_plane", l_mesh_new_plane },
        { "update_bounds", l_mesh_update_bounds },
        { "bounds_min", l_mesh_bounds_min },
        { "bounds_max", l_mesh_bounds_max },
        { "set_origin", l_mesh_set_origin },
        { "transform", l_mesh_transform },
        { "size", l_mesh_size },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_canvas[]
    = { { "new", l_canvas_new },
        { "get_texture", l_canvas_get_texture },
        { "get_size", l_canvas_get_size },
        { "set_clear_color", l_canvas_set_clear_color },
        { "set_wrap_mode", l_canvas_set_wrap_mode },
        { "text", l_canvas_text },
        { "rectangle", l_canvas_rectangle },
        { "blit", l_canvas_blit },
        { "blit_sliced", l_canvas_blit_sliced },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_graphics[]
    = { { "draw_line", l_graphics_draw_line },
        { "draw_dir", l_graphics_draw_dir },
        { "set_layer", l_graphics_set_layer },
        { "set_color", l_graphics_set_color },
        { "set_transform", l_graphics_set_transform },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_camera[]
    = { { "set_fov", l_camera_set_fov },
        { "set_near", l_camera_set_near },
        { "set_far", l_camera_set_far },
        { "set_aspect", l_camera_set_aspect },
        { "reset_aspect", l_camera_reset_aspect },
        { "set_ortho", l_camera_set_ortho },
        { "set_ortho_size", l_camera_set_ortho_size },
        { "get_projection", l_camera_get_projection },
        { "unproject", l_camera_unproject },
        { "set_render_mask", l_camera_set_render_mask },
        { "get_render_mask", l_camera_get_render_mask },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_staticmesh[]
    = { { "set_mesh", l_staticmesh_set_mesh },
        { "get_mesh", l_staticmesh_get_mesh },
        { "set_texture", l_staticmesh_set_texture },
        { "get_texture", l_staticmesh_get_texture },
        { "set_colormap", l_staticmesh_set_colormap },
        { "set_render_layer", l_staticmesh_set_render_layer },
        { "get_render_layer", l_staticmesh_get_render_layer },
        { "set_draw_bounds", l_staticmesh_set_draw_bounds },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_colormap[] = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_layer[]    = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_vertex[]   = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_color[]    = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_anchor[]   = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_rigidbody[]
    = { { "set_velocity", l_rigidbody_set_velocity }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_collider[]
    = { { "set_sphere", l_collider_set_sphere },
        { "set_aabb", l_collider_set_aabb },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_physics[]
    = { { "raycast", l_physics_raycast },
        { "set_ground_height", l_physics_set_ground_height },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_stylesheet[] = { { "new", l_stylesheet_new },
                                                  { "set", l_stylesheet_set },
                                                  { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_gui[]        = { { "new", l_gui_new },
                                                  { "push_style", l_gui_push_style },
                                                  { "pop_style", l_gui_pop_style },
                                                  { "button", l_gui_button },
                                                  { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_api (void)
{
    lua_State *L = nux_lua_state();
    lua_newtable(L);
    luaL_setfuncs(L, lib_stat, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "FPS");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "SCREEN_WIDTH");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "SCREEN_HEIGHT");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "TIMESTAMP");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "stat");
    lua_newtable(L);
    luaL_setfuncs(L, lib_time, 0);
    lua_setglobal(L, "time");
    lua_newtable(L);
    luaL_setfuncs(L, lib_random, 0);
    lua_setglobal(L, "random");
    lua_newtable(L);
    luaL_setfuncs(L, lib_arena, 0);
    lua_setglobal(L, "arena");
    lua_newtable(L);
    luaL_setfuncs(L, lib_logger, 0);
    lua_setglobal(L, "logger");
    lua_newtable(L);
    luaL_setfuncs(L, lib_resource, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "NULL");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ANY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "ARENA");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "LUA_MODULE");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "TEXTURE");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "MESH");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "PALETTE");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "VIEWPORT");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "CANVAS");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "FONT");
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "FILE");
    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "DISK");
    lua_pushinteger(L, 12);
    lua_setfield(L, -2, "SCENE");
    lua_pushinteger(L, 13);
    lua_setfield(L, -2, "QUERY");
    lua_pushinteger(L, 14);
    lua_setfield(L, -2, "EVENT");
    lua_pushinteger(L, 15);
    lua_setfield(L, -2, "INPUTMAP");
    lua_pushinteger(L, 16);
    lua_setfield(L, -2, "GUI");
    lua_pushinteger(L, 17);
    lua_setfield(L, -2, "STYLESHEET");
    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "resource");
    lua_newtable(L);
    luaL_setfuncs(L, lib_module, 0);
    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "NO_DATA_INITIALIZATION");
    lua_setglobal(L, "module");
    lua_newtable(L);
    luaL_setfuncs(L, lib_event, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "LUA");
    lua_setglobal(L, "event");
    lua_newtable(L);
    luaL_setfuncs(L, lib_io, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "READ");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "READ_WRITE");
    lua_setglobal(L, "io");
    lua_newtable(L);
    luaL_setfuncs(L, lib_mount, 0);
    lua_setglobal(L, "mount");
    lua_newtable(L);
    luaL_setfuncs(L, lib_file, 0);
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "file");
    lua_newtable(L);
    luaL_setfuncs(L, lib_error, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "NONE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "OUT_OF_MEMORY");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "INVALID_TEXTURE_SIZE");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "WASM_RUNTIME");
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "CART_EOF");
    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "CART_MOUNT");
    lua_setglobal(L, "error");
    lua_newtable(L);
    luaL_setfuncs(L, lib_log, 0);
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "DEBUG");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "INFO");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "WARNING");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ERROR");
    lua_setglobal(L, "log");
    lua_newtable(L);
    luaL_setfuncs(L, lib_name, 0);
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "name");
    lua_newtable(L);
    luaL_setfuncs(L, lib_system, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "PRE_UPDATE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "UPDATE");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "POST_UPDATE");
    lua_setglobal(L, "system");
    lua_newtable(L);
    luaL_setfuncs(L, lib_disk, 0);
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "disk");
    lua_newtable(L);
    luaL_setfuncs(L, lib_inputmap, 0);
    lua_setglobal(L, "inputmap");
    lua_newtable(L);
    luaL_setfuncs(L, lib_input, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "UNMAPPED");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "KEY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "MOUSE_BUTTON");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "MOUSE_AXIS");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "GAMEPAD_BUTTON");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "GAMEPAD_AXIS");
    lua_setglobal(L, "input");
    lua_newtable(L);
    luaL_setfuncs(L, lib_button, 0);
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "PRESSED");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "RELEASED");
    lua_setglobal(L, "button");
    lua_newtable(L);
    luaL_setfuncs(L, lib_key, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "SPACE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "APOSTROPHE");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "COMMA");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "MINUS");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "PERIOD");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "SLASH");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "NUM0");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "NUM1");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "NUM2");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "NUM3");
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "NUM4");
    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "NUM5");
    lua_pushinteger(L, 12);
    lua_setfield(L, -2, "NUM6");
    lua_pushinteger(L, 13);
    lua_setfield(L, -2, "NUM7");
    lua_pushinteger(L, 14);
    lua_setfield(L, -2, "NUM8");
    lua_pushinteger(L, 15);
    lua_setfield(L, -2, "NUM9");
    lua_pushinteger(L, 16);
    lua_setfield(L, -2, "SEMICOLON");
    lua_pushinteger(L, 17);
    lua_setfield(L, -2, "EQUAL");
    lua_pushinteger(L, 18);
    lua_setfield(L, -2, "A");
    lua_pushinteger(L, 19);
    lua_setfield(L, -2, "B");
    lua_pushinteger(L, 20);
    lua_setfield(L, -2, "C");
    lua_pushinteger(L, 21);
    lua_setfield(L, -2, "D");
    lua_pushinteger(L, 22);
    lua_setfield(L, -2, "E");
    lua_pushinteger(L, 23);
    lua_setfield(L, -2, "F");
    lua_pushinteger(L, 24);
    lua_setfield(L, -2, "G");
    lua_pushinteger(L, 25);
    lua_setfield(L, -2, "H");
    lua_pushinteger(L, 26);
    lua_setfield(L, -2, "I");
    lua_pushinteger(L, 27);
    lua_setfield(L, -2, "J");
    lua_pushinteger(L, 29);
    lua_setfield(L, -2, "K");
    lua_pushinteger(L, 30);
    lua_setfield(L, -2, "L");
    lua_pushinteger(L, 31);
    lua_setfield(L, -2, "M");
    lua_pushinteger(L, 32);
    lua_setfield(L, -2, "N");
    lua_pushinteger(L, 33);
    lua_setfield(L, -2, "O");
    lua_pushinteger(L, 34);
    lua_setfield(L, -2, "P");
    lua_pushinteger(L, 35);
    lua_setfield(L, -2, "Q");
    lua_pushinteger(L, 36);
    lua_setfield(L, -2, "R");
    lua_pushinteger(L, 37);
    lua_setfield(L, -2, "S");
    lua_pushinteger(L, 38);
    lua_setfield(L, -2, "T");
    lua_pushinteger(L, 39);
    lua_setfield(L, -2, "U");
    lua_pushinteger(L, 40);
    lua_setfield(L, -2, "V");
    lua_pushinteger(L, 41);
    lua_setfield(L, -2, "W");
    lua_pushinteger(L, 42);
    lua_setfield(L, -2, "X");
    lua_pushinteger(L, 43);
    lua_setfield(L, -2, "Y");
    lua_pushinteger(L, 44);
    lua_setfield(L, -2, "Z");
    lua_pushinteger(L, 45);
    lua_setfield(L, -2, "LEFT_BRACKET");
    lua_pushinteger(L, 46);
    lua_setfield(L, -2, "BACKSLASH");
    lua_pushinteger(L, 47);
    lua_setfield(L, -2, "RIGHT_BRACKET");
    lua_pushinteger(L, 48);
    lua_setfield(L, -2, "GRAVE_ACCENT");
    lua_pushinteger(L, 49);
    lua_setfield(L, -2, "ESCAPE");
    lua_pushinteger(L, 50);
    lua_setfield(L, -2, "ENTER");
    lua_pushinteger(L, 51);
    lua_setfield(L, -2, "TAB");
    lua_pushinteger(L, 52);
    lua_setfield(L, -2, "BACKSPACE");
    lua_pushinteger(L, 53);
    lua_setfield(L, -2, "INSERT");
    lua_pushinteger(L, 54);
    lua_setfield(L, -2, "DELETE");
    lua_pushinteger(L, 55);
    lua_setfield(L, -2, "RIGHT");
    lua_pushinteger(L, 56);
    lua_setfield(L, -2, "LEFT");
    lua_pushinteger(L, 57);
    lua_setfield(L, -2, "DOWN");
    lua_pushinteger(L, 58);
    lua_setfield(L, -2, "UP");
    lua_pushinteger(L, 59);
    lua_setfield(L, -2, "PAGE_UP");
    lua_pushinteger(L, 60);
    lua_setfield(L, -2, "PAGE_DOWN");
    lua_pushinteger(L, 61);
    lua_setfield(L, -2, "HOME");
    lua_pushinteger(L, 62);
    lua_setfield(L, -2, "END");
    lua_pushinteger(L, 63);
    lua_setfield(L, -2, "CAPS_LOCK");
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "SCROLL_LOCK");
    lua_pushinteger(L, 65);
    lua_setfield(L, -2, "NUM_LOCK");
    lua_pushinteger(L, 66);
    lua_setfield(L, -2, "PRINT_SCREEN");
    lua_pushinteger(L, 67);
    lua_setfield(L, -2, "PAUSE");
    lua_pushinteger(L, 68);
    lua_setfield(L, -2, "F1");
    lua_pushinteger(L, 69);
    lua_setfield(L, -2, "F2");
    lua_pushinteger(L, 70);
    lua_setfield(L, -2, "F3");
    lua_pushinteger(L, 71);
    lua_setfield(L, -2, "F4");
    lua_pushinteger(L, 72);
    lua_setfield(L, -2, "F5");
    lua_pushinteger(L, 73);
    lua_setfield(L, -2, "F6");
    lua_pushinteger(L, 74);
    lua_setfield(L, -2, "F7");
    lua_pushinteger(L, 75);
    lua_setfield(L, -2, "F8");
    lua_pushinteger(L, 76);
    lua_setfield(L, -2, "F9");
    lua_pushinteger(L, 77);
    lua_setfield(L, -2, "F10");
    lua_pushinteger(L, 78);
    lua_setfield(L, -2, "F11");
    lua_pushinteger(L, 79);
    lua_setfield(L, -2, "F12");
    lua_pushinteger(L, 80);
    lua_setfield(L, -2, "F13");
    lua_pushinteger(L, 81);
    lua_setfield(L, -2, "F14");
    lua_pushinteger(L, 82);
    lua_setfield(L, -2, "F15");
    lua_pushinteger(L, 83);
    lua_setfield(L, -2, "F16");
    lua_pushinteger(L, 84);
    lua_setfield(L, -2, "F17");
    lua_pushinteger(L, 85);
    lua_setfield(L, -2, "F18");
    lua_pushinteger(L, 86);
    lua_setfield(L, -2, "F19");
    lua_pushinteger(L, 87);
    lua_setfield(L, -2, "F20");
    lua_pushinteger(L, 88);
    lua_setfield(L, -2, "F21");
    lua_pushinteger(L, 89);
    lua_setfield(L, -2, "F22");
    lua_pushinteger(L, 90);
    lua_setfield(L, -2, "F23");
    lua_pushinteger(L, 91);
    lua_setfield(L, -2, "F24");
    lua_pushinteger(L, 92);
    lua_setfield(L, -2, "F25");
    lua_pushinteger(L, 93);
    lua_setfield(L, -2, "KP_0");
    lua_pushinteger(L, 94);
    lua_setfield(L, -2, "KP_1");
    lua_pushinteger(L, 95);
    lua_setfield(L, -2, "KP_2");
    lua_pushinteger(L, 96);
    lua_setfield(L, -2, "KP_3");
    lua_pushinteger(L, 97);
    lua_setfield(L, -2, "KP_4");
    lua_pushinteger(L, 98);
    lua_setfield(L, -2, "KP_5");
    lua_pushinteger(L, 99);
    lua_setfield(L, -2, "KP_6");
    lua_pushinteger(L, 100);
    lua_setfield(L, -2, "KP_7");
    lua_pushinteger(L, 101);
    lua_setfield(L, -2, "KP_8");
    lua_pushinteger(L, 102);
    lua_setfield(L, -2, "KP_9");
    lua_pushinteger(L, 103);
    lua_setfield(L, -2, "KP_DECIMAL");
    lua_pushinteger(L, 104);
    lua_setfield(L, -2, "KP_DIVIDE");
    lua_pushinteger(L, 105);
    lua_setfield(L, -2, "KP_MULTIPLY");
    lua_pushinteger(L, 106);
    lua_setfield(L, -2, "KP_SUBTRACT");
    lua_pushinteger(L, 107);
    lua_setfield(L, -2, "KP_ADD");
    lua_pushinteger(L, 108);
    lua_setfield(L, -2, "KP_ENTER");
    lua_pushinteger(L, 109);
    lua_setfield(L, -2, "KP_EQUAL");
    lua_pushinteger(L, 110);
    lua_setfield(L, -2, "LEFT_SHIFT");
    lua_pushinteger(L, 111);
    lua_setfield(L, -2, "LEFT_CONTROL");
    lua_pushinteger(L, 112);
    lua_setfield(L, -2, "LEFT_ALT");
    lua_pushinteger(L, 113);
    lua_setfield(L, -2, "LEFT_SUPER");
    lua_pushinteger(L, 114);
    lua_setfield(L, -2, "RIGHT_SHIFT");
    lua_pushinteger(L, 115);
    lua_setfield(L, -2, "RIGHT_CONTROL");
    lua_pushinteger(L, 116);
    lua_setfield(L, -2, "RIGHT_ALT");
    lua_pushinteger(L, 117);
    lua_setfield(L, -2, "RIGHT_SUPER");
    lua_pushinteger(L, 118);
    lua_setfield(L, -2, "MENU");
    lua_setglobal(L, "key");
    lua_newtable(L);
    luaL_setfuncs(L, lib_mouse, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "BUTTON_LEFT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "BUTTON_RIGHT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "BUTTON_MIDDLE");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "WHEEL_UP");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "WHEEL_DOWN");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "MOTION_RIGHT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "MOTION_LEFT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "MOTION_DOWN");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "MOTION_UP");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "SCROLL_UP");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "SCROLL_DOWN");
    lua_setglobal(L, "mouse");
    lua_newtable(L);
    luaL_setfuncs(L, lib_gamepad, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "A");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "X");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "Y");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "B");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "DPAD_UP");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "DPAD_DOWN");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "DPAD_LEFT");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "DPAD_RIGHT");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "SHOULDER_LEFT");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "SHOULDER_RIGHT");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "LSTICK_LEFT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LSTICK_RIGHT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "LSTICK_UP");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "LSTICK_DOWN");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "RSTICK_LEFT");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "RSTICK_RIGHT");
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "RSTICK_UP");
    lua_pushinteger(L, 7);
    lua_setfield(L, -2, "RSTICK_DOWN");
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "LTRIGGER");
    lua_pushinteger(L, 9);
    lua_setfield(L, -2, "RTRIGGER");
    lua_setglobal(L, "gamepad");
    lua_newtable(L);
    luaL_setfuncs(L, lib_cursor, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "UP");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "DOWN");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "LEFT");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "RIGHT");
    lua_setglobal(L, "cursor");
    lua_newtable(L);
    luaL_setfuncs(L, lib_controller, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "MODE_SELECTION");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "MODE_CURSOR");
    lua_setglobal(L, "controller");
    lua_newtable(L);
    luaL_setfuncs(L, lib_lua, 0);
    lua_setglobal(L, "lua");
    lua_newtable(L);
    luaL_setfuncs(L, lib_component, 0);
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "TRANSFORM");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "CAMERA");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "STATICMESH");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "RIGIDBODY");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "COLLIDER");
    lua_pushinteger(L, 16);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "component");
    lua_newtable(L);
    luaL_setfuncs(L, lib_transform, 0);
    lua_setglobal(L, "transform");
    lua_newtable(L);
    luaL_setfuncs(L, lib_query, 0);
    lua_setglobal(L, "query");
    lua_newtable(L);
    luaL_setfuncs(L, lib_scene, 0);
    lua_setglobal(L, "scene");
    lua_newtable(L);
    luaL_setfuncs(L, lib_node, 0);
    lua_setglobal(L, "node");
    lua_newtable(L);
    luaL_setfuncs(L, lib_viewport, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "HIDDEN");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "FIXED");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "FIXED_BEST_FIT");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "STRETCH_KEEP_ASPECT");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "STRETCH");
    lua_setglobal(L, "viewport");
    lua_newtable(L);
    luaL_setfuncs(L, lib_texture, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "IMAGE_RGBA");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "IMAGE_INDEX");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "RENDER_TARGET");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "WRAP_CLAMP");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "WRAP_REPEAT");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "WRAP_MIRROR");
    lua_setglobal(L, "texture");
    lua_newtable(L);
    luaL_setfuncs(L, lib_palette, 0);
    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "SIZE");
    lua_setglobal(L, "palette");
    lua_newtable(L);
    luaL_setfuncs(L, lib_mesh, 0);
    lua_setglobal(L, "mesh");
    lua_newtable(L);
    luaL_setfuncs(L, lib_canvas, 0);
    lua_setglobal(L, "canvas");
    lua_newtable(L);
    luaL_setfuncs(L, lib_graphics, 0);
    lua_setglobal(L, "graphics");
    lua_newtable(L);
    luaL_setfuncs(L, lib_camera, 0);
    lua_setglobal(L, "camera");
    lua_newtable(L);
    luaL_setfuncs(L, lib_staticmesh, 0);
    lua_setglobal(L, "staticmesh");
    lua_newtable(L);
    luaL_setfuncs(L, lib_colormap, 0);
    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "SIZE");
    lua_setglobal(L, "colormap");
    lua_newtable(L);
    luaL_setfuncs(L, lib_layer, 0);
    lua_pushinteger(L, 0x1);
    lua_setfield(L, -2, "DEFAULT");
    lua_setglobal(L, "layer");
    lua_newtable(L);
    luaL_setfuncs(L, lib_vertex, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TRIANGLES");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LINES");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "POINTS");
    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "POSITION");
    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "TEXCOORD");
    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "COLOR");
    lua_setglobal(L, "vertex");
    lua_newtable(L);
    luaL_setfuncs(L, lib_color, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TRANSPARENT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "WHITE");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "RED");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "GREEN");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "BLUE");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "BACKGROUND");
    lua_setglobal(L, "color");
    lua_newtable(L);
    luaL_setfuncs(L, lib_anchor, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "CENTER");
    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "TOP");
    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "BOTTOM");
    lua_pushinteger(L, 1 << 3);
    lua_setfield(L, -2, "LEFT");
    lua_pushinteger(L, 1 << 4);
    lua_setfield(L, -2, "RIGHT");
    lua_setglobal(L, "anchor");
    lua_newtable(L);
    luaL_setfuncs(L, lib_rigidbody, 0);
    lua_setglobal(L, "rigidbody");
    lua_newtable(L);
    luaL_setfuncs(L, lib_collider, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "SPHERE");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "AABB");
    lua_setglobal(L, "collider");
    lua_newtable(L);
    luaL_setfuncs(L, lib_physics, 0);
    lua_setglobal(L, "physics");
    lua_newtable(L);
    luaL_setfuncs(L, lib_stylesheet, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "BUTTON_PRESSED");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "BUTTON_RELEASED");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "BUTTON_HOVERED");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "CHECKBOX_CHECKED");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "CHECKBOX_UNCHECKED");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "CURSOR");
    lua_setglobal(L, "stylesheet");
    lua_newtable(L);
    luaL_setfuncs(L, lib_gui, 0);
    lua_setglobal(L, "gui");
    return NUX_SUCCESS;
}