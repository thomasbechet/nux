#include "internal.h"

#include "lua_api.c.inc"

nux_status_t
nux_lua_load_conf (nux_ctx_t *ctx)
{
    lua_State *L = luaL_newstate();
    NUX_CHECKM(L, "Failed to initialize lua VM", return NUX_FAILURE);
    if (luaL_dofile(L, "cart.lua") != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(L, -1));
        goto error;
    }

    if (!lua_istable(L, -1))
    {
        NUX_ERROR("Return value from cart.lua is not a table");
        goto error;
    }

    if (!lua_getfield(L, -1, "name"))
    {
        NUX_ERROR("missing field");
        goto error;
    }
    lua_isstring(L, -1);
    NUX_INFO("%s", lua_tostring(L, -1));

    lua_close(L);
    return NUX_SUCCESS;

error:
    lua_close(L);
    return NUX_FAILURE;
}
nux_status_t
nux_lua_init (nux_ctx_t *ctx)
{
    // Initialize Lua VM
    ctx->L = luaL_newstate();
    NUX_CHECKM(ctx->L, "Failed to initialize lua VM", return NUX_FAILURE);

    // Set ctx variable
    lua_pushlightuserdata(ctx->L, ctx); // TODO: per thread ctx
    lua_rawseti(ctx->L, LUA_REGISTRYINDEX, 1);

    // Load api
    luaL_openlibs(ctx->L);
    nux_register_lua(ctx);

    if (luaL_dofile(ctx->L, "main.lua") != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
    }

    lua_getglobal(ctx->L, "nux");
    lua_getfield(ctx->L, -1, "init");
    if (lua_pcall(ctx->L, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
    }
    lua_pop(ctx->L, 1);

    return NUX_SUCCESS;
}
void
nux_lua_free (nux_ctx_t *ctx)
{
    if (ctx->L)
    {
        lua_close(ctx->L);
    }
}
void
nux_lua_tick (nux_ctx_t *ctx)
{
    lua_getglobal(ctx->L, "nux");
    lua_getfield(ctx->L, -1, "tick");
    if (lua_pcall(ctx->L, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
    }
    lua_pop(ctx->L, 1);
}
