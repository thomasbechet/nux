#include "internal.h"

#include "vm/lualib.h"
#include "vm/lauxlib.h"
#include "lua_api.c.inc"
#include "lua_ext.c.inc"

static nux_status_t
lua_register_ext (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;

    if (luaL_dostring(L, lua_ext_code) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
    }

    return NUX_SUCCESS;
}
static nux_status_t
call_nux_function (nux_ctx_t *ctx, const nux_c8_t *name)
{
    lua_getglobal(ctx->L, "nux");
    lua_getfield(ctx->L, -1, name);
    if (lua_pcall(ctx->L, 0, 0, 0))
    {
        nux_error(ctx, "%s", lua_tostring(ctx->L, -1));
        lua_pop(ctx->L, 1);
        return NUX_FAILURE;
    }
    lua_pop(ctx->L, 1);
    return NUX_SUCCESS;
}

nux_status_t
nux_lua_configure (nux_ctx_t *ctx)
{
    // Initialize Lua VM
    ctx->L = luaL_newstate(ctx);
    NUX_ENSURE(ctx->L, return NUX_FAILURE, "failed to initialize lua state");

    // Create nux table
    lua_newtable(ctx->L);
    lua_setglobal(ctx->L, "nux");
    luaL_openlibs(ctx->L);

    // Configuration callback
    if (luaL_dofile(ctx->L, "init.lua") != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }

    // Call nux.conf
    lua_getglobal(ctx->L, "nux");
    lua_getfield(ctx->L, -1, "conf");
    if (lua_pcall(ctx->L, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }
    lua_pop(ctx->L, 1);

    return NUX_SUCCESS;

error:
    lua_close(ctx->L);
    return NUX_FAILURE;
}
nux_status_t
nux_lua_init (nux_ctx_t *ctx)
{
    // Register complete nux api
    nux_lua_register_base(ctx);
    lua_register_ext(ctx);

    // Call nux.init
    return call_nux_function(ctx, "init");
}
void
nux_lua_free (nux_ctx_t *ctx)
{
    if (ctx->L)
    {
        lua_close(ctx->L);
    }
}
nux_status_t
nux_lua_tick (nux_ctx_t *ctx)
{
    return call_nux_function(ctx, "tick");
}
