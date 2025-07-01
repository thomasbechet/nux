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
    ctx->lua_state = luaL_newstate();
    NUX_CHECKM(
        ctx->lua_state, "Failed to initialize lua state", return NUX_FAILURE);

    // Set ctx variable
    lua_pushlightuserdata(ctx->lua_state, ctx); // TODO: per thread ctx
    lua_rawseti(ctx->lua_state, LUA_REGISTRYINDEX, 1);

    // Load api
    luaL_openlibs(ctx->lua_state);
    nux_lua_register_base(ctx);
    nux_lua_register_ext(ctx);

    if (luaL_dofile(ctx->lua_state, "main.lua") != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua_state, -1));
    }

    lua_getglobal(ctx->lua_state, "nux");
    lua_getfield(ctx->lua_state, -1, "init");
    if (lua_pcall(ctx->lua_state, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua_state, -1));
    }
    lua_pop(ctx->lua_state, 1);

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

static int
l_scene_parse (lua_State *L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
    nux_ctx_t *ctx = lua_touserdata(L, -1);
    nux_u32_t  ret = nux_scene_parse(ctx, L);
    lua_pushinteger(L, ret);
    return 1;
}
nux_status_t
nux_lua_register_ext (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua_state;

    lua_getglobal(L, "nux");
    NUX_ASSERT(lua_istable(L, -1));

    lua_getfield(L, -1, "scene");
    NUX_ASSERT(lua_istable(L, -1));
    static const struct luaL_Reg lib_scene[]
        = { { "parse", l_scene_parse }, { NUX_NULL, NUX_NULL } };
    luaL_setfuncs(L, lib_scene, 0);
    lua_pop(L, 2);

    return NUX_SUCCESS;
}
