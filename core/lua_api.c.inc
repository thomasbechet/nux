#include "internal.h"

static int
l_core_trace (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t      *env  = lua_touserdata(L, -1);
    const nux_c8_t *text = luaL_checkstring(L, 1);

    nux_trace(env, text);

    return 0;
}
static int
l_core_stat (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env  = lua_touserdata(L, -1);
    nux_stat_t info = luaL_checkinteger(L, 1);

    nux_u32_t ret =

        nux_stat(env, info);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_time (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  ret =

        nux_time(env);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_core_dt (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  ret =

        nux_dt(env);

    lua_pushnumber(L, ret);
    return 1;
}
static int
l_core_frame (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_frame(env);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_rectfill (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);

    nux_i32_t y0 = luaL_checkinteger(L, 2);

    nux_i32_t x1 = luaL_checkinteger(L, 3);

    nux_i32_t y1 = luaL_checkinteger(L, 4);

    nux_u8_t c = luaL_checkinteger(L, 5);

    nux_rectfill(env, x0, y0, x1, y1, c);

    return 0;
}
static int
l_core_trifill (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);

    nux_i32_t y0 = luaL_checkinteger(L, 2);

    nux_i32_t x1 = luaL_checkinteger(L, 3);

    nux_i32_t y1 = luaL_checkinteger(L, 4);

    nux_i32_t x2 = luaL_checkinteger(L, 5);

    nux_i32_t y2 = luaL_checkinteger(L, 6);

    nux_u8_t c = luaL_checkinteger(L, 7);

    nux_trifill(env, x0, y0, x1, y1, x2, y2, c);

    return 0;
}
static int
l_core_pal (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u8_t color = luaL_checkinteger(L, 2);

    nux_pal(env, index, color);

    return 0;
}
static int
l_core_palt (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u8_t   c   = luaL_checkinteger(L, 1);

    nux_palt(env, c);

    return 0;
}
static int
l_core_palr (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_palr(env);

    return 0;
}
static int
l_core_palc (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u8_t ret =

        nux_palc(env, index);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_cls (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u32_t  color = luaL_checkinteger(L, 1);

    nux_cls(env, color);

    return 0;
}
static int
l_core_pset (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x   = luaL_checkinteger(L, 1);

    nux_i32_t y = luaL_checkinteger(L, 2);

    nux_u8_t c = luaL_checkinteger(L, 3);

    nux_pset(env, x, y, c);

    return 0;
}
static int
l_core_cget (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u32_t ret =

        nux_cget(env, index);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_cset (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u16_t c = luaL_checkinteger(L, 2);

    nux_cset(env, index, c);

    return 0;
}

static int
l_graphics_text (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x   = luaL_checkinteger(L, 1);

    nux_i32_t y = luaL_checkinteger(L, 2);

    const nux_c8_t *text = luaL_checkstring(L, 3);

    nux_u8_t c = luaL_checkinteger(L, 3);

    nux_graphics_text(env, x, y, text, c);

    return 0;
}
static int
l_graphics_print (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t      *env  = lua_touserdata(L, -1);
    const nux_c8_t *text = luaL_checkstring(L, 1);

    nux_u8_t c = luaL_checkinteger(L, 1);

    nux_graphics_print(env, text, c);

    return 0;
}
static int
l_graphics_cursor_x (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  ret =

        nux_graphics_cursor_x(env);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_graphics_cursor_y (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  ret =

        nux_graphics_cursor_y(env);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_graphics_cursor (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x   = luaL_checkinteger(L, 1);

    nux_i32_t y = luaL_checkinteger(L, 2);

    nux_graphics_cursor(env, x, y);

    return 0;
}
static int
l_graphics_line (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);

    nux_i32_t y0 = luaL_checkinteger(L, 2);

    nux_i32_t x1 = luaL_checkinteger(L, 3);

    nux_i32_t y1 = luaL_checkinteger(L, 4);

    nux_u8_t color = luaL_checkinteger(L, 5);

    nux_graphics_line(env, x0, y0, x1, y1, color);

    return 0;
}
static int
l_graphics_circle (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  xm  = luaL_checkinteger(L, 1);

    nux_i32_t ym = luaL_checkinteger(L, 2);

    nux_i32_t r = luaL_checkinteger(L, 3);

    nux_u8_t c = luaL_checkinteger(L, 4);

    nux_graphics_circle(env, xm, ym, r, c);

    return 0;
}
static int
l_graphics_rectangle (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);

    nux_i32_t y0 = luaL_checkinteger(L, 2);

    nux_i32_t x1 = luaL_checkinteger(L, 3);

    nux_i32_t y1 = luaL_checkinteger(L, 4);

    nux_u8_t c = luaL_checkinteger(L, 5);

    nux_graphics_rectangle(env, x0, y0, x1, y1, c);

    return 0;
}
static int
l_graphics_set_render_target (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);

    nux_graphics_set_render_target(env, id);

    return 0;
}
static int
l_graphics_blit (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);

    nux_graphics_blit(env, id);

    return 0;
}

static int
l_input_button (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_u32_t ret =

        nux_input_button(env, player);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_input_axis (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_axis_t axis = luaL_checkinteger(L, 2);

    nux_f32_t ret =

        nux_input_axis(env, player, axis);

    lua_pushnumber(L, ret);
    return 1;
}

static int
l_button_pressed (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret =

        nux_button_pressed(env, player, button);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_released (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret =

        nux_button_released(env, player, button);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_pressed (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret =

        nux_button_just_pressed(env, player, button);

    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_released (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret =

        nux_button_just_released(env, player, button);

    lua_pushboolean(L, ret);
    return 1;
}

static int
l_texture_new (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t         *env    = lua_touserdata(L, -1);
    nux_texture_type_t format = luaL_checkinteger(L, 1);

    nux_u32_t w = luaL_checkinteger(L, 2);

    nux_u32_t h = luaL_checkinteger(L, 3);

    nux_u32_t ret =

        nux_texture_new(env, format, w, h);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_arena_new (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_arena_new(env);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_arena_reset (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);

    nux_arena_reset(env, id);

    return 0;
}
static int
l_arena_use (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);

    nux_status_t ret =

        nux_arena_use(env, id);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_arena_active (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_arena_active(env);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_arena_dump (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);

    nux_arena_dump(env, id);

    return 0;
}

static int
l_scene_new (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_scene_new(env);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_scene_draw (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u32_t  scene = luaL_checkinteger(L, 1);

    nux_u32_t camera = luaL_checkinteger(L, 2);

    nux_scene_draw(env, scene, camera);

    return 0;
}

static int
l_entity_new (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u32_t  world = luaL_checkinteger(L, 1);

    nux_u32_t ret =

        nux_entity_new(env, world);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_transform_add (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_transform_add(env, entity);

    return 0;
}
static int
l_transform_remove (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_transform_remove(env, entity);

    return 0;
}
static int
l_transform_set_parent (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_u32_t parent = luaL_checkinteger(L, 2);

    nux_transform_set_parent(env, entity, parent);

    return 0;
}
static int
l_transform_get_parent (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_u32_t ret =

        nux_transform_get_parent(env, entity);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_transform_get_translation (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_get_translation(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_get_scale (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_get_scale(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_get_global_translation (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_get_global_translation(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_get_global_scale (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_get_global_scale(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_forward (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_forward(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_backward (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_backward(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_left (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_left(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_right (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_right(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_up (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_up(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_down (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t ret =

        nux_transform_down(env, entity);

    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_set_translation (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t position;
    position.x = luaL_checknumber(L, 2);
    position.y = luaL_checknumber(L, 3);
    position.z = luaL_checknumber(L, 4);

    nux_transform_set_translation(env, entity, position);

    return 0;
}
static int
l_transform_set_scale (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t scale;
    scale.x = luaL_checknumber(L, 2);
    scale.y = luaL_checknumber(L, 3);
    scale.z = luaL_checknumber(L, 4);

    nux_transform_set_scale(env, entity, scale);

    return 0;
}
static int
l_transform_rotate_y (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_y(env, entity, angle);

    return 0;
}
static int
l_transform_look_at (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_v3_t center;
    center.x = luaL_checknumber(L, 2);
    center.y = luaL_checknumber(L, 3);
    center.z = luaL_checknumber(L, 4);

    nux_transform_look_at(env, entity, center);

    return 0;
}

static int
l_camera_add (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_camera_add(env, entity);

    return 0;
}
static int
l_camera_remove (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_camera_remove(env, entity);

    return 0;
}
static int
l_camera_set_fov (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_f32_t fov = luaL_checknumber(L, 2);

    nux_camera_set_fov(env, entity, fov);

    return 0;
}

static int
l_staticmesh_add (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_staticmesh_add(env, entity);

    return 0;
}
static int
l_staticmesh_remove (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_staticmesh_remove(env, entity);

    return 0;
}
static int
l_staticmesh_set_mesh (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  entity = luaL_checkinteger(L, 1);

    nux_u32_t mesh = luaL_checkinteger(L, 2);

    nux_staticmesh_set_mesh(env, entity, mesh);

    return 0;
}

static int
l_mesh_new (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env  = lua_touserdata(L, -1);
    nux_u32_t  capa = luaL_checkinteger(L, 1);

    nux_u32_t ret =

        nux_mesh_new(env, capa);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_mesh_gen_cube (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  sx  = luaL_checknumber(L, 1);

    nux_f32_t sy = luaL_checknumber(L, 2);

    nux_f32_t sz = luaL_checknumber(L, 3);

    nux_u32_t ret =

        nux_mesh_gen_cube(env, sx, sy, sz);

    lua_pushinteger(L, ret);
    return 1;
}
static int
l_mesh_load (lua_State *L)
{

    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t      *env  = lua_touserdata(L, -1);
    const nux_c8_t *path = luaL_checkstring(L, 1);

    nux_u32_t ret =

        nux_mesh_load(env, path);

    lua_pushinteger(L, ret);
    return 1;
}

static const struct luaL_Reg lib_core[]
    = { { "trace", l_core_trace },     { "stat", l_core_stat },
        { "time", l_core_time },       { "dt", l_core_dt },
        { "frame", l_core_frame },     { "rectfill", l_core_rectfill },
        { "trifill", l_core_trifill }, { "pal", l_core_pal },
        { "palt", l_core_palt },       { "palr", l_core_palr },
        { "palc", l_core_palc },       { "cls", l_core_cls },
        { "pset", l_core_pset },       { "cget", l_core_cget },
        { "cset", l_core_cset },       { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_graphics[]
    = { { "text", l_graphics_text },
        { "print", l_graphics_print },
        { "cursor_x", l_graphics_cursor_x },
        { "cursor_y", l_graphics_cursor_y },
        { "cursor", l_graphics_cursor },
        { "line", l_graphics_line },
        { "circle", l_graphics_circle },
        { "rectangle", l_graphics_rectangle },
        { "set_render_target", l_graphics_set_render_target },
        { "blit", l_graphics_blit },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_input[] = { { "button", l_input_button },
                                             { "axis", l_input_axis },
                                             { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_button[]
    = { { "pressed", l_button_pressed },
        { "released", l_button_released },
        { "just_pressed", l_button_just_pressed },
        { "just_released", l_button_just_released },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_texture[]
    = { { "new", l_texture_new }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_arena[]
    = { { "new", l_arena_new },   { "reset", l_arena_reset },
        { "use", l_arena_use },   { "active", l_arena_active },
        { "dump", l_arena_dump }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_scene[] = { { "new", l_scene_new },
                                             { "draw", l_scene_draw },
                                             { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_entity[]
    = { { "new", l_entity_new }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_transform[]
    = { { "add", l_transform_add },
        { "remove", l_transform_remove },
        { "set_parent", l_transform_set_parent },
        { "get_parent", l_transform_get_parent },
        { "get_translation", l_transform_get_translation },
        { "get_scale", l_transform_get_scale },
        { "get_global_translation", l_transform_get_global_translation },
        { "get_global_scale", l_transform_get_global_scale },
        { "forward", l_transform_forward },
        { "backward", l_transform_backward },
        { "left", l_transform_left },
        { "right", l_transform_right },
        { "up", l_transform_up },
        { "down", l_transform_down },
        { "set_translation", l_transform_set_translation },
        { "set_scale", l_transform_set_scale },
        { "rotate_y", l_transform_rotate_y },
        { "look_at", l_transform_look_at },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_camera[] = { { "add", l_camera_add },
                                              { "remove", l_camera_remove },
                                              { "set_fov", l_camera_set_fov },
                                              { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_staticmesh[]
    = { { "add", l_staticmesh_add },
        { "remove", l_staticmesh_remove },
        { "set_mesh", l_staticmesh_set_mesh },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_mesh[] = { { "new", l_mesh_new },
                                            { "gen_cube", l_mesh_gen_cube },
                                            { "load", l_mesh_load },
                                            { NUX_NULL, NUX_NULL } };

nux_status_t
nux_register_lua (nux_instance_t *inst)
{
    lua_newtable(inst->L);

    luaL_setfuncs(inst->L, lib_core, 0);

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_graphics, 0);
    lua_setfield(inst->L, -2, "graphics");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_input, 0);
    lua_setfield(inst->L, -2, "input");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_button, 0);
    lua_setfield(inst->L, -2, "button");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_texture, 0);
    lua_setfield(inst->L, -2, "texture");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_arena, 0);
    lua_setfield(inst->L, -2, "arena");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_scene, 0);
    lua_setfield(inst->L, -2, "scene");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_entity, 0);
    lua_setfield(inst->L, -2, "entity");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_transform, 0);
    lua_setfield(inst->L, -2, "transform");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_camera, 0);
    lua_setfield(inst->L, -2, "camera");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_staticmesh, 0);
    lua_setfield(inst->L, -2, "staticmesh");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_mesh, 0);
    lua_setfield(inst->L, -2, "mesh");

    lua_pushinteger(inst->L, 512);
    lua_setfield(inst->L, -2, "CANVAS_WIDTH");

    lua_pushinteger(inst->L, 320);
    lua_setfield(inst->L, -2, "CANVAS_HEIGHT");

    lua_pushinteger(inst->L, 4096);
    lua_setfield(inst->L, -2, "TEXTURE_WIDTH");

    lua_pushinteger(inst->L, 4096);
    lua_setfield(inst->L, -2, "TEXTURE_HEIGHT");

    lua_pushinteger(inst->L, 4);
    lua_setfield(inst->L, -2, "PLAYER_MAX");

    lua_pushinteger(inst->L, 10);
    lua_setfield(inst->L, -2, "BUTTON_MAX");

    lua_pushinteger(inst->L, 6);
    lua_setfield(inst->L, -2, "AXIS_MAX");

    lua_pushinteger(inst->L, 64);
    lua_setfield(inst->L, -2, "NAME_MAX");

    lua_pushinteger(inst->L, 256);
    lua_setfield(inst->L, -2, "PALETTE_SIZE");

    lua_pushinteger(inst->L, 256);
    lua_setfield(inst->L, -2, "COLORMAP_SIZE");

    lua_pushinteger(inst->L, 1024);
    lua_setfield(inst->L, -2, "GPU_COMMAND_SIZE");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "ERROR_NONE");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "ERROR_OUT_OF_MEMORY");

    lua_pushinteger(inst->L, 4);
    lua_setfield(inst->L, -2, "ERROR_INVALID_TEXTURE_SIZE");

    lua_pushinteger(inst->L, 8);
    lua_setfield(inst->L, -2, "ERROR_WASM_RUNTIME");

    lua_pushinteger(inst->L, 10);
    lua_setfield(inst->L, -2, "ERROR_CART_EOF");

    lua_pushinteger(inst->L, 11);
    lua_setfield(inst->L, -2, "ERROR_CART_MOUNT");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "SUCCESS");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "FAILURE");

    lua_pushinteger(inst->L, 6);
    lua_setfield(inst->L, -2, "VERTEX_TRIANGLES");

    lua_pushinteger(inst->L, 6);
    lua_setfield(inst->L, -2, "VERTEX_LINES");

    lua_pushinteger(inst->L, 6);
    lua_setfield(inst->L, -2, "VERTEX_POINTS");

    lua_pushinteger(inst->L, 1 << 0);
    lua_setfield(inst->L, -2, "VERTEX_POSITION");

    lua_pushinteger(inst->L, 1 << 1);
    lua_setfield(inst->L, -2, "VERTEX_UV");

    lua_pushinteger(inst->L, 1 << 2);
    lua_setfield(inst->L, -2, "VERTEX_COLOR");

    lua_pushinteger(inst->L, 1 << 3);
    lua_setfield(inst->L, -2, "VERTEX_INDICES");

    lua_pushinteger(inst->L, 1 << 0);
    lua_setfield(inst->L, -2, "BUTTON_A");

    lua_pushinteger(inst->L, 1 << 1);
    lua_setfield(inst->L, -2, "BUTTON_X");

    lua_pushinteger(inst->L, 1 << 2);
    lua_setfield(inst->L, -2, "BUTTON_Y");

    lua_pushinteger(inst->L, 1 << 3);
    lua_setfield(inst->L, -2, "BUTTON_B");

    lua_pushinteger(inst->L, 1 << 4);
    lua_setfield(inst->L, -2, "BUTTON_UP");

    lua_pushinteger(inst->L, 1 << 5);
    lua_setfield(inst->L, -2, "BUTTON_DOWN");

    lua_pushinteger(inst->L, 1 << 6);
    lua_setfield(inst->L, -2, "BUTTON_LEFT");

    lua_pushinteger(inst->L, 1 << 7);
    lua_setfield(inst->L, -2, "BUTTON_RIGHT");

    lua_pushinteger(inst->L, 1 << 8);
    lua_setfield(inst->L, -2, "BUTTON_LB");

    lua_pushinteger(inst->L, 1 << 9);
    lua_setfield(inst->L, -2, "BUTTON_RB");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "AXIS_LEFTX");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "AXIS_LEFTY");

    lua_pushinteger(inst->L, 2);
    lua_setfield(inst->L, -2, "AXIS_RIGHTX");

    lua_pushinteger(inst->L, 3);
    lua_setfield(inst->L, -2, "AXIS_RIGHTY");

    lua_pushinteger(inst->L, 4);
    lua_setfield(inst->L, -2, "AXIS_RT");

    lua_pushinteger(inst->L, 5);
    lua_setfield(inst->L, -2, "AXIS_LT");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "STAT_FPS");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "STAT_SCREEN_WIDTH");

    lua_pushinteger(inst->L, 2);
    lua_setfield(inst->L, -2, "STAT_SCREEN_HEIGHT");

    lua_pushinteger(inst->L, 3);
    lua_setfield(inst->L, -2, "STAT_MAX");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "V3F");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "V3F_T2F");

    lua_pushinteger(inst->L, 2);
    lua_setfield(inst->L, -2, "V3F_T2F_N3F");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "TRIANGLES");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "LINES");

    lua_pushinteger(inst->L, 0);
    lua_setfield(inst->L, -2, "TEXTURE_IMAGE_RGBA");

    lua_pushinteger(inst->L, 1);
    lua_setfield(inst->L, -2, "TEXTURE_IMAGE_INDEX");

    lua_pushinteger(inst->L, 2);
    lua_setfield(inst->L, -2, "TEXTURE_RENDER_TARGET");

    lua_pushinteger(inst->L, 5);
    lua_setfield(inst->L, -2, "LOG_DEBUG");

    lua_pushinteger(inst->L, 4);
    lua_setfield(inst->L, -2, "LOG_INFO");

    lua_pushinteger(inst->L, 3);
    lua_setfield(inst->L, -2, "LOG_WARNING");

    lua_pushinteger(inst->L, 2);
    lua_setfield(inst->L, -2, "LOG_ERROR");

    lua_setglobal(inst->L, "nux");
    return NUX_SUCCESS;
}