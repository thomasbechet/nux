#include "internal.h"

static void
l_checkerror (lua_State *L, nux_ctx_t *ctx)
{
    if (!nux_error_get_status(ctx))
    {
        luaL_error(L, nux_error_get_message(ctx));
    }
}

static int
l_lua_load (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    const nux_c8_t *path = luaL_checkstring(L, 2);

    nux_res_t ret = nux_lua_load(ctx, arena, path);
    l_checkerror(L, ctx);
    if (ret)
    {
        lua_pushinteger(L, (nux_intptr_t)ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static const struct luaL_Reg lib_lua[]
    = { { "load", l_lua_load }, { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_lua (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);
    luaL_setfuncs(L, lib_lua, 0);
    lua_setfield(L, -2, "lua");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}