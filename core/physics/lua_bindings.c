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
l_rigidbody_add (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_rigidbody_add(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_rigidbody_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_rigidbody_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_rigidbody_set_velocity (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t velocity;
    velocity.x = luaL_checknumber(L, 2);
    velocity.y = luaL_checknumber(L, 3);
    velocity.z = luaL_checknumber(L, 4);
    nux_rigidbody_set_velocity(ctx, e, velocity);
    l_checkerror(L, ctx);
    return 0;
}

static const struct luaL_Reg lib_rigidbody[]
    = { { "add", l_rigidbody_add },
        { "remove", l_rigidbody_remove },
        { "set_velocity", l_rigidbody_set_velocity },
        { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_physics (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);
    luaL_setfuncs(L, lib_rigidbody, 0);
    lua_setfield(L, -2, "rigidbody");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}