#include "internal.h"

#include "lua_data.c.inc"

nux_status_t
nux_lua_init (nux_ctx_t *ctx)
{
    // Initialize Lua VM
    ctx->L = luaL_newstate(ctx);
    NUX_ASSERT(lua_getuserdata(ctx->L) == ctx);
    NUX_ENSURE(ctx->L, return NUX_FAILURE, "failed to initialize lua state");

    // Create nux table
    lua_newtable(ctx->L);
    lua_setglobal(ctx->L, NUX_TABLE);

    // Register API
    luaL_openlibs(ctx->L);
    nux_lua_open_base(ctx);
    nux_lua_open_graphics(ctx);
    nux_lua_open_ecs(ctx);

    // Register lua scripts
    if (luaL_dostring(ctx->L, lua_data_code) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }

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
nux_status_t
nux_lua_configure (nux_ctx_t *ctx, const nux_c8_t *path, nux_config_t *config)
{
    // Load init script
    if (luaL_dofile(ctx->L, path) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }

    // Call nux.conf
    NUX_CHECK(nux_lua_invoke(ctx, NUX_FUNC_CONF), return NUX_FAILURE);

    return NUX_SUCCESS;
}
nux_status_t
nux_lua_invoke (nux_ctx_t *ctx, const nux_c8_t *func)
{
    lua_State   *L      = ctx->L;
    nux_status_t status = NUX_SUCCESS;
    if (lua_getglobal(L, NUX_TABLE) != LUA_TTABLE)
    {
        lua_pop(L, 1);
        return NUX_SUCCESS;
    }
    if (lua_getfield(L, -1, func) != LUA_TFUNCTION)
    {
        lua_pop(L, 2); // pop field and table
        return NUX_SUCCESS;
    }
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) // consume field
    {
        nux_error(ctx, "%s", lua_tostring(L, -1));
        lua_pop(L, 1); // pop table
        return NUX_FAILURE;
    }
    lua_pop(L, 1); // pop table
    return NUX_SUCCESS;
}
