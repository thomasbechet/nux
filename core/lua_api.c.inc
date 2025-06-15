#include "internal.h"

static int
l_base_stat (lua_State *L)
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
l_base_time (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  ret =

        nux_time(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_base_dt (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_f32_t  ret =

        nux_dt(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_base_frame (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_frame(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_base_rectfill (lua_State *L)
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
l_base_trifill (lua_State *L)
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
l_base_pal (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);
    nux_u8_t   color = luaL_checkinteger(L, 2);
    nux_pal(env, index, color);

    return 0;
}

static int
l_base_palt (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u8_t   c   = luaL_checkinteger(L, 1);
    nux_palt(env, c);

    return 0;
}

static int
l_base_palr (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_palr(env);

    return 0;
}

static int
l_base_palc (lua_State *L)
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
l_base_cls (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u32_t  color = luaL_checkinteger(L, 1);
    nux_cls(env, color);

    return 0;
}

static int
l_base_pset (lua_State *L)
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
l_base_cget (lua_State *L)
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
l_base_cset (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u8_t   index = luaL_checkinteger(L, 1);
    nux_u16_t  c     = luaL_checkinteger(L, 2);
    nux_cset(env, index, c);

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
    nux_i32_t  y   = luaL_checkinteger(L, 2);
    nux_graphics_cursor(env, x, y);

    return 0;
}

static int
l_graphics_line (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_i32_t  x0    = luaL_checkinteger(L, 1);
    nux_i32_t  y0    = luaL_checkinteger(L, 2);
    nux_i32_t  x1    = luaL_checkinteger(L, 3);
    nux_i32_t  y1    = luaL_checkinteger(L, 4);
    nux_u8_t   color = luaL_checkinteger(L, 5);
    nux_graphics_line(env, x0, y0, x1, y1, color);

    return 0;
}

static int
l_graphics_circle (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  xm  = luaL_checkinteger(L, 1);
    nux_i32_t  ym  = luaL_checkinteger(L, 2);
    nux_i32_t  r   = luaL_checkinteger(L, 3);
    nux_u8_t   c   = luaL_checkinteger(L, 4);
    nux_graphics_circle(env, xm, ym, r, c);

    return 0;
}

static int
l_graphics_rectangle (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_i32_t  x0  = luaL_checkinteger(L, 1);
    nux_i32_t  y0  = luaL_checkinteger(L, 2);
    nux_i32_t  x1  = luaL_checkinteger(L, 3);
    nux_i32_t  y1  = luaL_checkinteger(L, 4);
    nux_u8_t   c   = luaL_checkinteger(L, 5);
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
    nux_u32_t  ret =

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
    nux_axis_t axis   = luaL_checkinteger(L, 2);
    nux_f32_t  ret =

        nux_input_axis(env, player, axis);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_texture_new (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t         *env    = lua_touserdata(L, -1);
    nux_texture_type_t format = luaL_checkinteger(L, 1);
    nux_u32_t          w      = luaL_checkinteger(L, 2);
    nux_u32_t          h      = luaL_checkinteger(L, 3);
    nux_u32_t          ret =

        nux_texture_new(env, format, w, h);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_arena_new (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env  = lua_touserdata(L, -1);
    nux_u32_t  capa = luaL_checkinteger(L, 1);
    nux_u32_t  ret =

        nux_arena_new(env, capa);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_arena_reset (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_arena_reset(env);

    return 0;
}

static int
l_arena_set_active (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t   *env = lua_touserdata(L, -1);
    nux_u32_t    id  = luaL_checkinteger(L, 1);
    nux_status_t ret =

        nux_arena_set_active(env, id);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_world_new (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    nux_u32_t  ret =

        nux_world_new(env);

    lua_pushinteger(L, ret);
    return 1;
}

static int
l_entity_new (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env   = lua_touserdata(L, -1);
    nux_u32_t  world = luaL_checkinteger(L, 1);
    nux_u32_t  ret =

        nux_entity_new(env, world);

    lua_pushinteger(L, ret);
    return 1;
}

static const struct luaL_Reg lib_base[] = {

    { "stat", l_base_stat },

    { "time", l_base_time },

    { "dt", l_base_dt },

    { "frame", l_base_frame },

    { "rectfill", l_base_rectfill },

    { "trifill", l_base_trifill },

    { "pal", l_base_pal },

    { "palt", l_base_palt },

    { "palr", l_base_palr },

    { "palc", l_base_palc },

    { "cls", l_base_cls },

    { "pset", l_base_pset },

    { "cget", l_base_cget },

    { "cset", l_base_cset },

    { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_graphics[] = {

    { "cursor_x", l_graphics_cursor_x },

    { "cursor_y", l_graphics_cursor_y },

    { "cursor", l_graphics_cursor },

    { "line", l_graphics_line },

    { "circle", l_graphics_circle },

    { "rectangle", l_graphics_rectangle },

    { "set_render_target", l_graphics_set_render_target },

    { "blit", l_graphics_blit },

    { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_input[] = {

    { "button", l_input_button },

    { "axis", l_input_axis },

    { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_texture[] = {

    { "new", l_texture_new },

    { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_arena[] = {

    { "new", l_arena_new },

    { "reset", l_arena_reset },

    { "set_active", l_arena_set_active },

    { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_world[] = {

    { "new", l_world_new },

    { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_entity[] = {

    { "new", l_entity_new },

    { NUX_NULL, NUX_NULL }
};

nux_status_t
nux_register_lua (nux_instance_t *inst)
{
    lua_newtable(inst->L);

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_base, 0);
    lua_setfield(inst->L, -2, "base");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_graphics, 0);
    lua_setfield(inst->L, -2, "graphics");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_input, 0);
    lua_setfield(inst->L, -2, "input");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_texture, 0);
    lua_setfield(inst->L, -2, "texture");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_arena, 0);
    lua_setfield(inst->L, -2, "arena");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_world, 0);
    lua_setfield(inst->L, -2, "world");

    lua_newtable(inst->L);
    luaL_setfuncs(inst->L, lib_entity, 0);
    lua_setfield(inst->L, -2, "entity");

    lua_setglobal(inst->L, "nux");
    return NUX_SUCCESS;
}