#include "internal.h"

#include "luavm/lualib.h"
#include "luavm/lauxlib.h"
#include "lua_api.c.inc"
#include "lua_ext.c.inc"

static nux_status_t
dofile (nux_ctx_t *ctx, lua_State *L, const nux_c8_t *path)
{
    if (luaL_dofile(L, path) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
static nux_status_t
lua_register_ext (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua_state;

    if (luaL_dostring(L, lua_ext_code) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua_state, -1));
    }

    return NUX_SUCCESS;
}

nux_status_t
nux_lua_load_conf (nux_ctx_t *ctx)
{
    lua_State *L = luaL_newstate(ctx);
    NUX_CHECKM(L, return NUX_FAILURE, "failed to initialize lua VM");

    NUX_CHECK(dofile(ctx, L, "cart.lua"), goto error);
    if (!lua_istable(L, -1))
    {
        NUX_ERROR("return value from cart.lua is not a table");
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
    ctx->lua_state = luaL_newstate(ctx);
    NUX_CHECKM(
        ctx->lua_state, return NUX_FAILURE, "failed to initialize lua state");

    // Load api
    luaL_openlibs(ctx->lua_state);
    nux_lua_register_base(ctx);
    lua_register_ext(ctx);

    return NUX_SUCCESS;
}
void
nux_lua_free (nux_ctx_t *ctx)
{
    if (ctx->lua_state)
    {
        lua_close(ctx->lua_state);
    }
}
nux_status_t
nux_lua_start (nux_ctx_t *ctx)
{
    NUX_CHECK(dofile(ctx, ctx->lua_state, "main.lua"), return NUX_FAILURE);

    lua_getglobal(ctx->lua_state, "nux");
    lua_getfield(ctx->lua_state, -1, "init");
    if (lua_pcall(ctx->lua_state, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua_state, -1));
        return NUX_FAILURE;
    }
    lua_pop(ctx->lua_state, 1);

    return NUX_SUCCESS;
}
void
nux_lua_tick (nux_ctx_t *ctx)
{
    lua_getglobal(ctx->lua_state, "nux");
    lua_getfield(ctx->lua_state, -1, "tick");
    if (lua_pcall(ctx->lua_state, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua_state, -1));
    }
    lua_pop(ctx->lua_state, 1);
}
