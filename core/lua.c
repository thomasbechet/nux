#include "internal.h"

#include "lua_api.c.inc"
#include "lua_ext.c.inc"

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

nux_status_t
nux_lua_register_ext (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua_state;

    if (luaL_dostring(L, lua_ext_code) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua_state, -1));
    }

    return NUX_SUCCESS;
}

// static nux_v3_t
// parse_v3 (lua_State *L)
// {
//     lua_geti(L, -1, 1);
//     float x = luaL_checknumber(L, -1);
//     lua_geti(L, -2, 2);
//     float y = luaL_checknumber(L, -1);
//     lua_geti(L, -3, 3);
//     float    z = luaL_checknumber(L, -1);
//     nux_v3_t v = nux_v3(x, y, z);
//     lua_pop(L, 3);
//     return v;
// }
// nux_u32_t
// try_parse_u32 (lua_State      *L,
//                nux_i32_t       idx,
//                const nux_c8_t *k,
//                nux_u32_t       default_value)
// {
//     nux_u32_t ret = default_value;
//     lua_getfield(L, idx, k);
//     if (!lua_isnil(L, -1))
//     {
//         ret = luaL_checkinteger(L, -1);
//     }
//     lua_pop(L, 1);
//     return ret;
// }
// nux_f32_t
// nux_lua_try_parse_f32 (lua_State      *L,
//                        nux_i32_t       idx,
//                        const nux_c8_t *k,
//                        nux_f32_t       default_value)
// {
//     nux_f32_t ret = default_value;
//     lua_getfield(L, idx, k);
//     if (!lua_isnil(L, -1))
//     {
//         ret = luaL_checknumber(L, -1);
//     }
//     lua_pop(L, 1);
//     return ret;
// }
// nux_v3_t
// nux_lua_try_parse_v3 (lua_State      *L,
//                       nux_i32_t       idx,
//                       const nux_c8_t *k,
//                       nux_v3_t        default_value)
// {
//     nux_v3_t ret = default_value;
//     lua_getfield(L, idx, k);
//     if (!lua_isnil(L, -1))
//     {
//         ret = parse_v3(L);
//     }
//     lua_pop(L, 1);
//     return ret;
// }
