#include "nux_internal.h"

static void
l_checkerror (lua_State *L, nux_ctx_t *ctx)
{
    if (!nux_error_get_status(ctx))
    {
        luaL_error(L, nux_error_get_message(ctx));
    }
}

static int
l_physics_shoot (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_v3_t   pos;
    pos.x = luaL_checknumber(L, 1);
    pos.y = luaL_checknumber(L, 2);
    pos.z = luaL_checknumber(L, 3);

    nux_v3_t dir;
    dir.x = luaL_checknumber(L, 4);
    dir.y = luaL_checknumber(L, 5);
    dir.z = luaL_checknumber(L, 6);
    nux_physics_shoot(ctx, pos, dir);
    l_checkerror(L, ctx);
    return 0;
}

static const struct luaL_Reg lib_physics[]
    = { { "shoot", l_physics_shoot }, { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_physics (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);
    luaL_setfuncs(L, lib_physics, 0);
    lua_setfield(L, -2, "physics");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}