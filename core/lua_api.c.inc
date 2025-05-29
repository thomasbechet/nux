#include "internal.h"

static int
l_trace (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_dbgi32 (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_dbgf32 (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_stat (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_tricount (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_time (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_dt (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_frame (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_rectfill (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_trifill (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_text (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_print (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_cursorx (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_cursory (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_cursor (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_line (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_circ (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_rect (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_cameye (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_camcenter (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_camup (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_camviewport (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_camfov (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_mesh (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_mesh_wire (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_draw_cube (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_draw_plane (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_pal (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_palt (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_palr (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_palc (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_cls (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_pset (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_cget (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_cset (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}
static int
l_button (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_axis (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_new_texture (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 1;
}
static int
l_bind_texture (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_env_t *env = lua_touserdata(L, -1);
    lua_pushnumber(L, nux_frame(env));
    return 0;
}

static const struct luaL_Reg nuxlib[] = {

    { "trace", l_trace },
    { "dbgi32", l_dbgi32 },
    { "dbgf32", l_dbgf32 },
    { "stat", l_stat },
    { "tricount", l_tricount },
    { "time", l_time },
    { "dt", l_dt },
    { "frame", l_frame },
    { "rectfill", l_rectfill },
    { "trifill", l_trifill },
    { "text", l_text },
    { "print", l_print },
    { "cursorx", l_cursorx },
    { "cursory", l_cursory },
    { "cursor", l_cursor },
    { "line", l_line },
    { "circ", l_circ },
    { "rect", l_rect },
    { "cameye", l_cameye },
    { "camcenter", l_camcenter },
    { "camup", l_camup },
    { "camviewport", l_camviewport },
    { "camfov", l_camfov },
    { "mesh", l_mesh },
    { "mesh_wire", l_mesh_wire },
    { "draw_cube", l_draw_cube },
    { "draw_plane", l_draw_plane },
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
    { "bind_texture", l_bind_texture },
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