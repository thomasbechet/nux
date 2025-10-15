#include "internal.h"

static void
l_checkerror (lua_State *L)
{
    if (!nux_error_get_status())
    {
        luaL_error(L, nux_error_get_message());
    }
}

static int
l_core_stat (lua_State *L)
{
    nux_stat_t info = luaL_checknumber(L, 1);
    nux_u32_t  ret  = nux_stat(info);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_random (lua_State *L)
{
    nux_u32_t ret = nux_random();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_random01 (lua_State *L)
{
    nux_f32_t ret = nux_random01();
    l_checkerror(L);

    lua_pushnumber(L, ret);
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
l_time_timestamp (lua_State *L)
{
    nux_u64_t ret = nux_time_timestamp();
    l_checkerror(L);

    lua_pushinteger(L, ret);
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
l_arena_reset (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));

    nux_arena_reset(arena);
    l_checkerror(L);

    return 0;
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
l_arena_memory_capacity (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_u32_t ret = nux_arena_memory_capacity(arena);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
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
l_arena_core (lua_State *L)
{
    const nux_arena_t *ret = nux_arena_core();
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
l_arena_frame (lua_State *L)
{
    const nux_arena_t *ret = nux_arena_frame();
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
l_resource_path (lua_State *L)
{
    nux_rid_t       rid = (nux_rid_t)luaL_checknumber(L, 1);
    const nux_c8_t *ret = nux_resource_path(rid);
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
l_resource_set_name (lua_State *L)
{
    nux_rid_t       rid  = (nux_rid_t)luaL_checknumber(L, 1);
    const nux_c8_t *name = luaL_checkstring(L, 2);

    nux_resource_set_name(rid, name);
    l_checkerror(L);

    return 0;
}
static int
l_resource_name (lua_State *L)
{
    nux_rid_t       rid = (nux_rid_t)luaL_checknumber(L, 1);
    const nux_c8_t *ret = nux_resource_name(rid);
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
l_resource_arena (lua_State *L)
{
    nux_rid_t          rid = (nux_rid_t)luaL_checknumber(L, 1);
    const nux_arena_t *ret = nux_resource_arena(rid);
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
l_resource_find (lua_State *L)
{
    const nux_c8_t *name = luaL_checkstring(L, 1);
    nux_rid_t       ret  = nux_resource_find(name);
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
l_log_set_level (lua_State *L)
{
    nux_log_level_t level = luaL_checknumber(L, 1);

    nux_log_set_level(level);
    l_checkerror(L);

    return 0;
}
static int
l_button_state (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_u32_t ret        = nux_button_state(controller);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_button_pressed (lua_State *L)
{
    nux_u32_t    controller = luaL_checknumber(L, 1);
    nux_button_t button     = luaL_checknumber(L, 2);
    nux_b32_t    ret        = nux_button_pressed(controller, button);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_released (lua_State *L)
{
    nux_u32_t    controller = luaL_checknumber(L, 1);
    nux_button_t button     = luaL_checknumber(L, 2);
    nux_b32_t    ret        = nux_button_released(controller, button);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_pressed (lua_State *L)
{
    nux_u32_t    controller = luaL_checknumber(L, 1);
    nux_button_t button     = luaL_checknumber(L, 2);
    nux_b32_t    ret        = nux_button_just_pressed(controller, button);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_released (lua_State *L)
{
    nux_u32_t    controller = luaL_checknumber(L, 1);
    nux_button_t button     = luaL_checknumber(L, 2);
    nux_b32_t    ret        = nux_button_just_released(controller, button);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_axis_value (lua_State *L)
{
    nux_u32_t  controller = luaL_checknumber(L, 1);
    nux_axis_t axis       = luaL_checknumber(L, 2);
    nux_f32_t  ret        = nux_axis_value(controller, axis);
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_cursor_get (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_v2_t  ret        = nux_cursor_get(controller);
    l_checkerror(L);

    nux_lua_push_vec2(L, ret);
    return 1;
}
static int
l_cursor_set (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_f32_t x          = luaL_checknumber(L, 2);
    nux_f32_t y          = luaL_checknumber(L, 3);

    nux_cursor_set(controller, x, y);
    l_checkerror(L);

    return 0;
}
static int
l_cursor_x (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_f32_t ret        = nux_cursor_x(controller);
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_cursor_y (lua_State *L)
{
    nux_u32_t controller = luaL_checknumber(L, 1);
    nux_f32_t ret        = nux_cursor_y(controller);
    l_checkerror(L);

    lua_pushnumber(L, ret);
    return 1;
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
l_io_cart_end (lua_State *L)
{
    nux_status_t ret = nux_io_cart_end();
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
l_color_rgba (lua_State *L)
{
    nux_u8_t r   = luaL_checknumber(L, 1);
    nux_u8_t g   = luaL_checknumber(L, 2);
    nux_u8_t b   = luaL_checknumber(L, 3);
    nux_u8_t a   = luaL_checknumber(L, 4);
    nux_v4_t ret = nux_color_rgba(r, g, b, a);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
    return 1;
}
static int
l_color_hex (lua_State *L)
{
    nux_u32_t hex = luaL_checknumber(L, 1);
    nux_v4_t  ret = nux_color_hex(hex);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
    return 1;
}
static int
l_color_to_hex (lua_State *L)
{
    nux_v4_t  color = nux_lua_check_vec4(L, 1);
    nux_u32_t ret   = nux_color_to_hex(color);
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_color_to_srgb (lua_State *L)
{
    nux_v4_t color = nux_lua_check_vec4(L, 1);
    nux_v4_t ret   = nux_color_to_srgb(color);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
    return 1;
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
l_transform_add (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_transform_add(e);
    l_checkerror(L);

    return 0;
}
static int
l_transform_remove (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_transform_remove(e);
    l_checkerror(L);

    return 0;
}
static int
l_transform_get_local_translation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_get_local_translation(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_get_local_rotation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_q4_t  ret = nux_transform_get_local_rotation(e);
    l_checkerror(L);

    nux_lua_push_q4(L, ret);
    return 1;
}
static int
l_transform_get_local_scale (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_get_local_scale(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_get_translation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_get_translation(e);
    l_checkerror(L);

    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_get_rotation (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_q4_t  ret = nux_transform_get_rotation(e);
    l_checkerror(L);

    nux_lua_push_q4(L, ret);
    return 1;
}
static int
l_transform_get_scale (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  ret = nux_transform_get_scale(e);
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
l_scene_active (lua_State *L)
{
    const nux_scene_t *ret = nux_scene_active();
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
l_scene_count (lua_State *L)
{
    nux_u32_t ret = nux_scene_count();
    l_checkerror(L);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_scene_clear (lua_State *L)
{

    nux_scene_clear();
    l_checkerror(L);

    return 0;
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
l_node_root (lua_State *L)
{
    nux_nid_t ret = nux_node_root();
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
    nux_v4_t extent = nux_lua_check_vec4(L, 2);

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
l_viewport_get_target_size (lua_State *L)
{
    nux_viewport_t *vp
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_v2_t ret = nux_viewport_get_target_size(vp);
    l_checkerror(L);

    nux_lua_push_vec2(L, ret);
    return 1;
}
static int
l_viewport_get_render_extent (lua_State *L)
{
    nux_viewport_t *viewport
        = nux_resource_check(NUX_RESOURCE_VIEWPORT, luaL_checkinteger(L, 1));
    nux_v4_t ret = nux_viewport_get_render_extent(viewport);
    l_checkerror(L);

    nux_lua_push_vec4(L, ret);
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
l_palette_default (lua_State *L)
{
    const nux_palette_t *ret = nux_palette_default();
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
    const nux_mesh_t      *ret        = nux_mesh_new(arena, capa, attributes);
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
l_graphics_draw_line_tr (lua_State *L)
{
    nux_m4_t  tr    = nux_lua_check_mat4(L, 1);
    nux_v3_t  a     = nux_lua_check_vec3(L, 2);
    nux_v3_t  b     = nux_lua_check_vec3(L, 3);
    nux_u32_t color = luaL_checknumber(L, 4);

    nux_graphics_draw_line_tr(tr, a, b, color);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_draw_line (lua_State *L)
{
    nux_v3_t  a     = nux_lua_check_vec3(L, 1);
    nux_v3_t  b     = nux_lua_check_vec3(L, 2);
    nux_u32_t color = luaL_checknumber(L, 3);

    nux_graphics_draw_line(a, b, color);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_draw_dir (lua_State *L)
{
    nux_v3_t  origin = nux_lua_check_vec3(L, 1);
    nux_v3_t  dir    = nux_lua_check_vec3(L, 2);
    nux_f32_t length = luaL_checknumber(L, 3);
    nux_u32_t color  = luaL_checknumber(L, 4);

    nux_graphics_draw_dir(origin, dir, length, color);
    l_checkerror(L);

    return 0;
}
static int
l_graphics_screen (lua_State *L)
{
    const nux_texture_t *ret = nux_graphics_screen();
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
l_camera_add (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_camera_add(e);
    l_checkerror(L);

    return 0;
}
static int
l_camera_remove (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_camera_remove(e);
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
l_staticmesh_has (lua_State *L)
{
    nux_nid_t n   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_b32_t ret = nux_staticmesh_has(n);
    l_checkerror(L);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_staticmesh_add (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_staticmesh_add(e);
    l_checkerror(L);

    return 0;
}
static int
l_staticmesh_remove (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_staticmesh_remove(e);
    l_checkerror(L);

    return 0;
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
l_rigidbody_add (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_rigidbody_add(e);
    l_checkerror(L);

    return 0;
}
static int
l_rigidbody_remove (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_rigidbody_remove(e);
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
l_collider_add_sphere (lua_State *L)
{
    nux_nid_t e      = (nux_nid_t)luaL_checknumber(L, 1);
    nux_f32_t radius = luaL_checknumber(L, 2);

    nux_collider_add_sphere(e, radius);
    l_checkerror(L);

    return 0;
}
static int
l_collider_add_aabb (lua_State *L)
{
    nux_nid_t e   = (nux_nid_t)luaL_checknumber(L, 1);
    nux_v3_t  min = nux_lua_check_vec3(L, 2);
    nux_v3_t  max = nux_lua_check_vec3(L, 3);

    nux_collider_add_aabb(e, min, max);
    l_checkerror(L);

    return 0;
}
static int
l_collider_remove (lua_State *L)
{
    nux_nid_t e = (nux_nid_t)luaL_checknumber(L, 1);

    nux_collider_remove(e);
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
static const struct luaL_Reg lib_core[] = { { "stat", l_core_stat },
                                            { "random", l_core_random },
                                            { "random01", l_core_random01 },
                                            { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_time[] = { { "elapsed", l_time_elapsed },
                                            { "delta", l_time_delta },
                                            { "frame", l_time_frame },
                                            { "timestamp", l_time_timestamp },
                                            { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_arena[]
    = { { "new", l_arena_new },
        { "reset", l_arena_reset },
        { "memory_usage", l_arena_memory_usage },
        { "memory_capacity", l_arena_memory_capacity },
        { "block_count", l_arena_block_count },
        { "core", l_arena_core },
        { "frame", l_arena_frame },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_resource[]
    = { { "path", l_resource_path }, { "set_name", l_resource_set_name },
        { "name", l_resource_name }, { "arena", l_resource_arena },
        { "find", l_resource_find }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_log[]
    = { { "set_level", l_log_set_level }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_button[]
    = { { "state", l_button_state },
        { "pressed", l_button_pressed },
        { "released", l_button_released },
        { "just_pressed", l_button_just_pressed },
        { "just_released", l_button_just_released },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_axis[]
    = { { "value", l_axis_value }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_cursor[] = { { "get", l_cursor_get },
                                              { "set", l_cursor_set },
                                              { "x", l_cursor_x },
                                              { "y", l_cursor_y },
                                              { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_io[]
    = { { "cart_begin", l_io_cart_begin },
        { "cart_end", l_io_cart_end },
        { "write_cart_file", l_io_write_cart_file },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_color[]      = { { "rgba", l_color_rgba },
                                                  { "hex", l_color_hex },
                                                  { "to_hex", l_color_to_hex },
                                                  { "to_srgb", l_color_to_srgb },
                                                  { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_error[]      = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_stat[]       = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_controller[] = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_name[]       = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_disk[]       = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_lua[]
    = { { "load", l_lua_load }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_transform[]
    = { { "add", l_transform_add },
        { "remove", l_transform_remove },
        { "get_local_translation", l_transform_get_local_translation },
        { "get_local_rotation", l_transform_get_local_rotation },
        { "get_local_scale", l_transform_get_local_scale },
        { "get_translation", l_transform_get_translation },
        { "get_rotation", l_transform_get_rotation },
        { "get_scale", l_transform_get_scale },
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
    = { { "new", l_scene_new },       { "set_active", l_scene_set_active },
        { "active", l_scene_active }, { "count", l_scene_count },
        { "clear", l_scene_clear },   { "load_gltf", l_scene_load_gltf },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_node[]
    = { { "create", l_node_create },
        { "delete", l_node_delete },
        { "valid", l_node_valid },
        { "root", l_node_root },
        { "parent", l_node_parent },
        { "set_parent", l_node_set_parent },
        { "sibling", l_node_sibling },
        { "child", l_node_child },
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
        { "set_camera", l_viewport_set_camera },
        { "set_texture", l_viewport_set_texture },
        { "get_target_size", l_viewport_get_target_size },
        { "get_render_extent", l_viewport_get_render_extent },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_texture[]
    = { { "new", l_texture_new }, { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_palette[]
    = { { "new", l_palette_new },
        { "default", l_palette_default },
        { "set_active", l_palette_set_active },
        { "set_color", l_palette_set_color },
        { "get_color", l_palette_get_color },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_mesh[]
    = { { "new", l_mesh_new },
        { "new_cube", l_mesh_new_cube },
        { "update_bounds", l_mesh_update_bounds },
        { "bounds_min", l_mesh_bounds_min },
        { "bounds_max", l_mesh_bounds_max },
        { "set_origin", l_mesh_set_origin },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_canvas[]
    = { { "new", l_canvas_new },
        { "get_texture", l_canvas_get_texture },
        { "set_clear_color", l_canvas_set_clear_color },
        { "text", l_canvas_text },
        { "rectangle", l_canvas_rectangle },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_graphics[]
    = { { "draw_line_tr", l_graphics_draw_line_tr },
        { "draw_line", l_graphics_draw_line },
        { "draw_dir", l_graphics_draw_dir },
        { "screen", l_graphics_screen },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_camera[]
    = { { "add", l_camera_add },
        { "remove", l_camera_remove },
        { "set_fov", l_camera_set_fov },
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
    = { { "has", l_staticmesh_has },
        { "add", l_staticmesh_add },
        { "remove", l_staticmesh_remove },
        { "set_mesh", l_staticmesh_set_mesh },
        { "get_mesh", l_staticmesh_get_mesh },
        { "set_texture", l_staticmesh_set_texture },
        { "get_texture", l_staticmesh_get_texture },
        { "set_colormap", l_staticmesh_set_colormap },
        { "set_render_layer", l_staticmesh_set_render_layer },
        { "get_render_layer", l_staticmesh_get_render_layer },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_colormap[]  = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_primitive[] = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_vertex[]    = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_anchor[]    = { { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_rigidbody[]
    = { { "add", l_rigidbody_add },
        { "remove", l_rigidbody_remove },
        { "set_velocity", l_rigidbody_set_velocity },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_collider[]
    = { { "add_sphere", l_collider_add_sphere },
        { "add_aabb", l_collider_add_aabb },
        { "remove", l_collider_remove },
        { NUX_NULL, NUX_NULL } };
static const struct luaL_Reg lib_physics[]
    = { { "raycast", l_physics_raycast },
        { "set_ground_height", l_physics_set_ground_height },
        { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_api (void)
{
    lua_State *L = nux_lua_module()->L;
    lua_newtable(L);
    luaL_setfuncs(L, lib_core, 0);
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "SUCCESS");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "FAILURE");
    lua_setglobal(L, "core");
    lua_newtable(L);
    luaL_setfuncs(L, lib_time, 0);
    lua_setglobal(L, "time");
    lua_newtable(L);
    luaL_setfuncs(L, lib_arena, 0);
    lua_setglobal(L, "arena");
    lua_newtable(L);
    luaL_setfuncs(L, lib_resource, 0);
    lua_setglobal(L, "resource");
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
    luaL_setfuncs(L, lib_button, 0);
    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "MAX");
    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "A");
    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "X");
    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "Y");
    lua_pushinteger(L, 1 << 3);
    lua_setfield(L, -2, "B");
    lua_pushinteger(L, 1 << 4);
    lua_setfield(L, -2, "UP");
    lua_pushinteger(L, 1 << 5);
    lua_setfield(L, -2, "DOWN");
    lua_pushinteger(L, 1 << 6);
    lua_setfield(L, -2, "LEFT");
    lua_pushinteger(L, 1 << 7);
    lua_setfield(L, -2, "RIGHT");
    lua_pushinteger(L, 1 << 8);
    lua_setfield(L, -2, "LB");
    lua_pushinteger(L, 1 << 9);
    lua_setfield(L, -2, "RB");
    lua_setglobal(L, "button");
    lua_newtable(L);
    luaL_setfuncs(L, lib_axis, 0);
    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "MAX");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "LEFTX");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LEFTY");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "RIGHTX");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "RIGHTY");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "RT");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "LT");
    lua_setglobal(L, "axis");
    lua_newtable(L);
    luaL_setfuncs(L, lib_cursor, 0);
    lua_setglobal(L, "cursor");
    lua_newtable(L);
    luaL_setfuncs(L, lib_io, 0);
    lua_setglobal(L, "io");
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
    luaL_setfuncs(L, lib_controller, 0);
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "controller");
    lua_newtable(L);
    luaL_setfuncs(L, lib_name, 0);
    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "name");
    lua_newtable(L);
    luaL_setfuncs(L, lib_disk, 0);
    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "disk");
    lua_newtable(L);
    luaL_setfuncs(L, lib_lua, 0);
    lua_setglobal(L, "lua");
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
    luaL_setfuncs(L, lib_primitive, 0);
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TRIANGLES");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LINES");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "POINTS");
    lua_setglobal(L, "primitive");
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
    return NUX_SUCCESS;
}