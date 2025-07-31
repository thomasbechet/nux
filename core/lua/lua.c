#include "internal.h"

#include "externals/lua/lualib.h"
#include "externals/lua/lauxlib.h"
#include "lua_api.c.inc"
#include "lua_ext.c.inc"

#define NUX_TABLE     "nux"
#define NUX_FUNC_CONF "conf"
#define NUX_FUNC_INIT "init"
#define NUX_FUNC_TICK "tick"

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
try_call_function (nux_ctx_t *ctx, const nux_c8_t *name)
{
    lua_getglobal(ctx->L, NUX_TABLE);
    lua_getfield(ctx->L, -1, name);
    if (!lua_isnil(ctx->L, -1))
    {
        if (lua_pcall(ctx->L, 0, 0, 0))
        {
            nux_error(ctx, "%s", lua_tostring(ctx->L, -1));
            lua_pop(ctx->L, 1);
            return NUX_FAILURE;
        }
    }
    lua_pop(ctx->L, 1);
    return NUX_SUCCESS;
}

nux_status_t
nux_lua_configure (nux_ctx_t *ctx, const nux_config_t *config)
{
    // Initialize Lua VM
    ctx->L = luaL_newstate(ctx);
    NUX_ENSURE(ctx->L, return NUX_FAILURE, "failed to initialize lua state");

    // Create nux table
    lua_newtable(ctx->L);
    lua_setglobal(ctx->L, NUX_TABLE);
    luaL_openlibs(ctx->L);

    // Configuration callback
    if (luaL_dofile(ctx->L, config->init_script) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }

    // Call nux.conf
    NUX_CHECK(try_call_function(ctx, NUX_FUNC_CONF), return NUX_FAILURE);

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
    return try_call_function(ctx, NUX_FUNC_INIT);
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
    return try_call_function(ctx, NUX_FUNC_TICK);
}
