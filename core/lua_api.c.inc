#include "internal.h"

static int
l_stat (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env  = lua_touserdata(L, -1);
    nux_stat_t info = luaL_checkinteger(L, 1);
    nux_u32_t  ret =

        nux_stat(env, info);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_time (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  ret =

        nux_time(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_dt (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  ret =

        nux_dt(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_frame (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_frame(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_rectfill (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);
    nux_i32_t  y0  = luaL_checkinteger(L, 2);
    nux_i32_t  x1  = luaL_checkinteger(L, 3);
    nux_i32_t  y1  = luaL_checkinteger(L, 4);
    nux_u8_t   c   = luaL_checkinteger(L, 5);
    nux_rectfill(env, x0, y0, x1, y1, c);

    return 0;
}

static int
l_trifill (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);
    nux_i32_t  y0  = luaL_checkinteger(L, 2);
    nux_i32_t  x1  = luaL_checkinteger(L, 3);
    nux_i32_t  y1  = luaL_checkinteger(L, 4);
    nux_i32_t  x2  = luaL_checkinteger(L, 5);
    nux_i32_t  y2  = luaL_checkinteger(L, 6);
    nux_u8_t   c   = luaL_checkinteger(L, 7);
    nux_trifill(env, x0, y0, x1, y1, x2, y2, c);

    return 0;
}

static int
l_cursorx (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  ret =

        nux_cursorx(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_cursory (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  ret =

        nux_cursory(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_cursor (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x   = luaL_checkinteger(L, 1);
    nux_i32_t  y   = luaL_checkinteger(L, 2);
    nux_cursor(env, x, y);

    return 0;
}

static int
l_line (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_i32_t  x0    = luaL_checkinteger(L, 1);
    nux_i32_t  y0    = luaL_checkinteger(L, 2);
    nux_i32_t  x1    = luaL_checkinteger(L, 3);
    nux_i32_t  y1    = luaL_checkinteger(L, 4);
    nux_u8_t   color = luaL_checkinteger(L, 5);
    nux_line(env, x0, y0, x1, y1, color);

    return 0;
}

static int
l_circ (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  xm  = luaL_checkinteger(L, 1);
    nux_i32_t  ym  = luaL_checkinteger(L, 2);
    nux_i32_t  r   = luaL_checkinteger(L, 3);
    nux_u8_t   c   = luaL_checkinteger(L, 4);
    nux_circ(env, xm, ym, r, c);

    return 0;
}

static int
l_rect (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);
    nux_i32_t  y0  = luaL_checkinteger(L, 2);
    nux_i32_t  x1  = luaL_checkinteger(L, 3);
    nux_i32_t  y1  = luaL_checkinteger(L, 4);
    nux_u8_t   c   = luaL_checkinteger(L, 5);
    nux_rect(env, x0, y0, x1, y1, c);

    return 0;
}

static int
l_pal (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);
    nux_u8_t   color = luaL_checkinteger(L, 2);
    nux_pal(env, index, color);

    return 0;
}

static int
l_palt (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u8_t   c   = luaL_checkinteger(L, 1);
    nux_palt(env, c);

    return 0;
}

static int
l_palr (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_palr(env);

    return 0;
}

static int
l_palc (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);
    nux_u8_t   ret =

        nux_palc(env, index);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_cls (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u32_t  color = luaL_checkinteger(L, 1);
    nux_cls(env, color);

    return 0;
}

static int
l_pset (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x   = luaL_checkinteger(L, 1);
    nux_i32_t  y   = luaL_checkinteger(L, 2);
    nux_u8_t   c   = luaL_checkinteger(L, 3);
    nux_pset(env, x, y, c);

    return 0;
}

static int
l_cget (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);
    nux_u32_t  ret =

        nux_cget(env, index);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_cset (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);
    nux_u16_t  c     = luaL_checkinteger(L, 2);
    nux_cset(env, index, c);

    return 0;
}

static int
l_button (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);
    nux_u32_t  ret =

        nux_button(env, player);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_axis (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env    = lua_touserdata(L, -1);
    nux_u32_t  player = luaL_checkinteger(L, 1);
    nux_axis_t axis   = luaL_checkinteger(L, 2);
    nux_f32_t  ret =

        nux_axis(env, player, axis);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_new_texture (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t           *env    = lua_touserdata(L, -1);
    nux_texture_format_t format = luaL_checkinteger(L, 1);
    nux_u32_t            w      = luaL_checkinteger(L, 2);
    nux_u32_t            h      = luaL_checkinteger(L, 3);
    nux_u32_t            ret =

        nux_new_texture(env, format, w, h);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_new_render_target (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  w   = luaL_checkinteger(L, 1);
    nux_u32_t  h   = luaL_checkinteger(L, 2);
    nux_u32_t  ret =

        nux_new_render_target(env, w, h);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_set_render_target (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);
    nux_set_render_target(env, id);

    return 0;
}

static int
l_blit (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);
    nux_blit(env, id);

    return 0;
}

static int
l_new_arena (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env  = lua_touserdata(L, -1);
    nux_u32_t  capa = luaL_checkinteger(L, 1);
    nux_u32_t  ret =

        nux_new_arena(env, capa);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_reset_arena (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_reset_arena(env);

    return 0;
}

static int
l_set_arena (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  id  = luaL_checkinteger(L, 1);
    nux_set_arena(env, id);

    return 0;
}

static const struct luaL_Reg nuxlib[] = {

    { "stat", l_stat },

    { "time", l_time },

    { "dt", l_dt },

    { "frame", l_frame },

    { "rectfill", l_rectfill },

    { "trifill", l_trifill },

    { "cursorx", l_cursorx },

    { "cursory", l_cursory },

    { "cursor", l_cursor },

    { "line", l_line },

    { "circ", l_circ },

    { "rect", l_rect },

    { "pal", l_pal },

    { "palt", l_palt },

    { "palr", l_palr },

    { "palc", l_palc },

    { "cls", l_cls },

    { "pset", l_pset },

    { "cget", l_cget },

    { "cset", l_cset },

    { "button", l_button },

    { "axis", l_axis },

    { "new_texture", l_new_texture },

    { "new_render_target", l_new_render_target },

    { "set_render_target", l_set_render_target },

    { "blit", l_blit },

    { "new_arena", l_new_arena },

    { "reset_arena", l_reset_arena },

    { "set_arena", l_set_arena },

    { NUX_NULL, NUX_NULL }
};

nux_status_t
nux_register_lua (nux_instance_t *inst)
{
    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, nuxlib, 0);
    lua_setglobal(inst->L, "nux");
    return NUX_SUCCESS;
}