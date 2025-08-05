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
l_log_set_level (lua_State *L)
{
    nux_ctx_t      *ctx   = lua_getuserdata(L);
    nux_log_level_t level = luaL_checkinteger(L, 1);

    nux_log_set_level(ctx, level);
    l_checkerror(L, ctx);
    return 0;
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
l_core_time (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_f32_t  ret = nux_time(ctx);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}
static int
l_core_dt (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_f32_t  ret = nux_dt(ctx);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}
static int
l_core_frame (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_frame(ctx);
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
l_input_button (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_u32_t ret = nux_input_button(ctx, player);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_input_axis (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_axis_t axis = luaL_checkinteger(L, 2);

    nux_f32_t ret = nux_input_axis(ctx, player, axis);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}

static int
l_button_pressed (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_pressed(ctx, player, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_released (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_released(ctx, player, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_pressed (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_just_pressed(ctx, player, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_released (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_u32_t  player = luaL_checkinteger(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_just_released(ctx, player, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}

static int
l_arena_new (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_u32_t  capa = luaL_checkinteger(L, 1);

    nux_res_t ret = nux_arena_new(ctx, capa);
    l_checkerror(L, ctx);
    if (ret)
    {
        lua_pushnumber(L, (nux_intptr_t)ret);
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
l_arena_frame (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  ret = nux_arena_frame(ctx);
    l_checkerror(L, ctx);
    if (ret)
    {
        lua_pushnumber(L, (nux_intptr_t)ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static int
l_arena_scratch (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  ret = nux_arena_scratch(ctx);
    l_checkerror(L, ctx);
    if (ret)
    {
        lua_pushnumber(L, (nux_intptr_t)ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static int
l_io_cart_begin (lua_State *L)
{
    nux_ctx_t      *ctx  = lua_getuserdata(L);
    const nux_c8_t *path = luaL_checkstring(L, 1);

    nux_u32_t entry_count = luaL_checkinteger(L, 2);

    nux_status_t ret = nux_io_cart_begin(ctx, path, entry_count);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_io_cart_end (lua_State *L)
{
    nux_ctx_t   *ctx = lua_getuserdata(L);
    nux_status_t ret = nux_io_cart_end(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_io_write_cart_file (lua_State *L)
{
    nux_ctx_t      *ctx  = lua_getuserdata(L);
    const nux_c8_t *path = luaL_checkstring(L, 1);

    nux_status_t ret = nux_io_write_cart_file(ctx, path);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}

static const struct luaL_Reg lib_log[]
    = { { "set_level", l_log_set_level }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_core[]
    = { { "stat", l_core_stat },     { "time", l_core_time },
        { "dt", l_core_dt },         { "frame", l_core_frame },
        { "random", l_core_random }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_input[] = { { "button", l_input_button },
                                             { "axis", l_input_axis },
                                             { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_button[]
    = { { "pressed", l_button_pressed },
        { "released", l_button_released },
        { "just_pressed", l_button_just_pressed },
        { "just_released", l_button_just_released },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_arena[] = { { "new", l_arena_new },
                                             { "reset", l_arena_reset },
                                             { "frame", l_arena_frame },
                                             { "scratch", l_arena_scratch },
                                             { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_io[]
    = { { "cart_begin", l_io_cart_begin },
        { "cart_end", l_io_cart_end },
        { "write_cart_file", l_io_write_cart_file },
        { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_base (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);
    luaL_setfuncs(L, lib_log, 0);
    lua_setfield(L, -2, "log");

    luaL_setfuncs(L, lib_core, 0);

    lua_newtable(L);
    luaL_setfuncs(L, lib_input, 0);
    lua_setfield(L, -2, "input");

    lua_newtable(L);
    luaL_setfuncs(L, lib_button, 0);
    lua_setfield(L, -2, "button");

    lua_newtable(L);
    luaL_setfuncs(L, lib_arena, 0);
    lua_setfield(L, -2, "arena");

    lua_newtable(L);
    luaL_setfuncs(L, lib_io, 0);
    lua_setfield(L, -2, "io");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "PLAYER_MAX");

    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "BUTTON_MAX");

    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "AXIS_MAX");

    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "NAME_MAX");

    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "DISK_MAX");

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

    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "BUTTON_A");

    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "BUTTON_X");

    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "BUTTON_Y");

    lua_pushinteger(L, 1 << 3);
    lua_setfield(L, -2, "BUTTON_B");

    lua_pushinteger(L, 1 << 4);
    lua_setfield(L, -2, "BUTTON_UP");

    lua_pushinteger(L, 1 << 5);
    lua_setfield(L, -2, "BUTTON_DOWN");

    lua_pushinteger(L, 1 << 6);
    lua_setfield(L, -2, "BUTTON_LEFT");

    lua_pushinteger(L, 1 << 7);
    lua_setfield(L, -2, "BUTTON_RIGHT");

    lua_pushinteger(L, 1 << 8);
    lua_setfield(L, -2, "BUTTON_LB");

    lua_pushinteger(L, 1 << 9);
    lua_setfield(L, -2, "BUTTON_RB");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "AXIS_LEFTX");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "AXIS_LEFTY");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "AXIS_RIGHTX");

    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "AXIS_RIGHTY");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "AXIS_RT");

    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "AXIS_LT");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "STAT_FPS");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "STAT_SCREEN_WIDTH");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "STAT_SCREEN_HEIGHT");

    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "STAT_MAX");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "LOG_DEBUG");

    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "LOG_INFO");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "LOG_WARNING");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LOG_ERROR");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}