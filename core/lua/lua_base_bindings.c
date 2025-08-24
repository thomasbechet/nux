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
l_core_stat (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_stat_t info = luaL_checkinteger(L, 1);

    nux_u32_t ret = nux_stat(ctx, info);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_random (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_random(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}

static int
l_time_elapsed (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_f32_t  ret = nux_time_elapsed(ctx);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}
static int
l_time_delta (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_f32_t  ret = nux_time_delta(ctx);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}
static int
l_time_frame (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_time_frame(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_time_timestamp (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u64_t  ret = nux_time_timestamp(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}

static int
l_arena_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    const nux_c8_t *name = luaL_checkstring(L, 2);

    nux_res_t ret = nux_arena_new(ctx, arena, name);
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
static int
l_arena_reset (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_arena_reset(ctx, arena);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_arena_core (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  ret = nux_arena_core(ctx);
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
static int
l_arena_frame (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  ret = nux_arena_frame(ctx);
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

static const struct luaL_Reg lib_core[] = { { "stat", l_core_stat },
                                            { "random", l_core_random },
                                            { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_time[] = { { "elapsed", l_time_elapsed },
                                            { "delta", l_time_delta },
                                            { "frame", l_time_frame },
                                            { "timestamp", l_time_timestamp },
                                            { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_arena[] = { { "new", l_arena_new },
                                             { "reset", l_arena_reset },
                                             { "core", l_arena_core },
                                             { "frame", l_arena_frame },
                                             { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_base (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_getglobal(L, "nux");

    luaL_setfuncs(L, lib_core, 0);

    lua_newtable(L);
    luaL_setfuncs(L, lib_time, 0);
    lua_setfield(L, -2, "time");

    lua_newtable(L);
    luaL_setfuncs(L, lib_arena, 0);
    lua_setfield(L, -2, "arena");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "ERROR_NONE");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "ERROR_OUT_OF_MEMORY");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "ERROR_INVALID_TEXTURE_SIZE");

    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "ERROR_WASM_RUNTIME");

    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "ERROR_CART_EOF");

    lua_pushinteger(L, 11);
    lua_setfield(L, -2, "ERROR_CART_MOUNT");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "SUCCESS");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "FAILURE");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "STAT_FPS");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "STAT_SCREEN_WIDTH");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "STAT_SCREEN_HEIGHT");

    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "STAT_TIMESTAMP");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "STAT_MAX");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}