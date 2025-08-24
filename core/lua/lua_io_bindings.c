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
l_button_state (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_u32_t ret = nux_button_state(ctx, controller);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_button_pressed (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_pressed(ctx, controller, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_released (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_released(ctx, controller, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_pressed (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_just_pressed(ctx, controller, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_button_just_released (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_button_t button = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_button_just_released(ctx, controller, button);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}

static int
l_core_axis (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_axis_t axis = luaL_checkinteger(L, 2);

    nux_f32_t ret = nux_axis(ctx, controller, axis);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}

static int
l_cursor_x (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_f32_t ret = nux_cursor_x(ctx, controller);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}
static int
l_cursor_y (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_f32_t ret = nux_cursor_y(ctx, controller);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret);
    return 1;
}
static int
l_cursor_set (lua_State *L)
{
    nux_ctx_t *ctx        = lua_getuserdata(L);
    nux_u32_t  controller = luaL_checknumber(L, 1);

    nux_f32_t x = luaL_checknumber(L, 2);

    nux_f32_t y = luaL_checknumber(L, 3);

    nux_cursor_set(ctx, controller, x, y);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_io_cart_begin (lua_State *L)
{
    nux_ctx_t      *ctx  = lua_getuserdata(L);
    const nux_c8_t *path = luaL_checkstring(L, 1);

    nux_u32_t entry_count = luaL_checknumber(L, 2);

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

static const struct luaL_Reg lib_button[]
    = { { "state", l_button_state },
        { "pressed", l_button_pressed },
        { "released", l_button_released },
        { "just_pressed", l_button_just_pressed },
        { "just_released", l_button_just_released },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_core[]
    = { { "axis", l_core_axis }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_cursor[] = { { "x", l_cursor_x },
                                              { "y", l_cursor_y },
                                              { "set", l_cursor_set },
                                              { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_io[]
    = { { "cart_begin", l_io_cart_begin },
        { "cart_end", l_io_cart_end },
        { "write_cart_file", l_io_write_cart_file },
        { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_io (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);
    luaL_setfuncs(L, lib_log, 0);
    lua_setfield(L, -2, "log");

    lua_newtable(L);
    luaL_setfuncs(L, lib_button, 0);
    lua_setfield(L, -2, "button");

    luaL_setfuncs(L, lib_core, 0);

    lua_newtable(L);
    luaL_setfuncs(L, lib_cursor, 0);
    lua_setfield(L, -2, "cursor");

    lua_newtable(L);
    luaL_setfuncs(L, lib_io, 0);
    lua_setfield(L, -2, "io");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "CONTROLLER_MAX");

    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "BUTTON_MAX");

    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "AXIS_MAX");

    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "NAME_MAX");

    lua_pushinteger(L, 8);
    lua_setfield(L, -2, "DISK_MAX");

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