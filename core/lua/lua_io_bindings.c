#include "internal.h"



static void l_checkerror(lua_State *L, nux_ctx_t *ctx)
{
    if (!nux_error_get_status(ctx))
    {
        luaL_error(L, nux_error_get_message(ctx));
    }
}

static int l_log_set_level(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_log_level_t level = luaL_checkinteger(L, 1);
nux_log_set_level(ctx, level);
    l_checkerror(L, ctx);        return 0;}static int l_button_state(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
        nux_u32_t ret =
nux_button_state(ctx, controller);
    l_checkerror(L, ctx);        lua_pushinteger(L, ret);
        return 1;}static int l_button_pressed(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
            nux_button_t button = luaL_checkinteger(L, 2);
        nux_b32_t ret =
nux_button_pressed(ctx, controller, button);
    l_checkerror(L, ctx);        lua_pushboolean(L, ret);
        return 1;}static int l_button_released(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
            nux_button_t button = luaL_checkinteger(L, 2);
        nux_b32_t ret =
nux_button_released(ctx, controller, button);
    l_checkerror(L, ctx);        lua_pushboolean(L, ret);
        return 1;}static int l_button_just_pressed(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
            nux_button_t button = luaL_checkinteger(L, 2);
        nux_b32_t ret =
nux_button_just_pressed(ctx, controller, button);
    l_checkerror(L, ctx);        lua_pushboolean(L, ret);
        return 1;}static int l_button_just_released(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
            nux_button_t button = luaL_checkinteger(L, 2);
        nux_b32_t ret =
nux_button_just_released(ctx, controller, button);
    l_checkerror(L, ctx);        lua_pushboolean(L, ret);
        return 1;}static int l_axis_value(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
            nux_axis_t axis = luaL_checkinteger(L, 2);
        nux_f32_t ret =
nux_axis_value(ctx, controller, axis);
    l_checkerror(L, ctx);        lua_pushnumber(L, ret);
        return 1;}static int l_cursor_x(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
        nux_f32_t ret =
nux_cursor_x(ctx, controller);
    l_checkerror(L, ctx);        lua_pushnumber(L, ret);
        return 1;}static int l_cursor_y(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
        nux_f32_t ret =
nux_cursor_y(ctx, controller);
    l_checkerror(L, ctx);        lua_pushnumber(L, ret);
        return 1;}static int l_cursor_set(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            nux_u32_t controller = luaL_checknumber(L, 1);
            nux_f32_t x = luaL_checknumber(L, 2);
            nux_f32_t y = luaL_checknumber(L, 3);
nux_cursor_set(ctx, controller, x, y);
    l_checkerror(L, ctx);        return 0;}static int l_io_cart_begin(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            const nux_c8_t *path = luaL_checkstring(L, 1);
            nux_u32_t entry_count = luaL_checknumber(L, 2);
        nux_status_t ret =
nux_io_cart_begin(ctx, path, entry_count);
    l_checkerror(L, ctx);        lua_pushinteger(L, ret);
        return 1;}static int l_io_cart_end(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);        nux_status_t ret =
nux_io_cart_end(ctx);
    l_checkerror(L, ctx);        lua_pushinteger(L, ret);
        return 1;}static int l_io_write_cart_file(lua_State *L)
{    nux_ctx_t *ctx = lua_getuserdata(L);            const nux_c8_t *path = luaL_checkstring(L, 1);
        nux_status_t ret =
nux_io_write_cart_file(ctx, path);
    l_checkerror(L, ctx);        lua_pushinteger(L, ret);
        return 1;}
static const struct luaL_Reg lib_log[] = {
{ "set_level", l_log_set_level },{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_button[] = {
{ "state", l_button_state },{ "pressed", l_button_pressed },{ "released", l_button_released },{ "just_pressed", l_button_just_pressed },{ "just_released", l_button_just_released },{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_axis[] = {
{ "value", l_axis_value },{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_cursor[] = {
{ "x", l_cursor_x },{ "y", l_cursor_y },{ "set", l_cursor_set },{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_io[] = {
{ "cart_begin", l_io_cart_begin },{ "cart_end", l_io_cart_end },{ "write_cart_file", l_io_write_cart_file },{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_controller[] = {
{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_name[] = {
{ NUX_NULL, NUX_NULL }
};
static const struct luaL_Reg lib_disk[] = {
{ NUX_NULL, NUX_NULL }
};

nux_status_t nux_lua_open_io(nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_newtable(L);
    luaL_setfuncs(L, lib_log, 0);
    lua_pushinteger(L, 4); 
    lua_setfield(L, -2, "DEBUG");
    lua_pushinteger(L, 3); 
    lua_setfield(L, -2, "INFO");
    lua_pushinteger(L, 2); 
    lua_setfield(L, -2, "WARNING");
    lua_pushinteger(L, 1); 
    lua_setfield(L, -2, "ERROR");
    lua_setglobal(L, "log");
    lua_newtable(L);
    luaL_setfuncs(L, lib_button, 0);
    lua_pushinteger(L, 10); 
    lua_setfield(L, -2, "MAX");
    lua_pushinteger(L, 1 << 0); 
    lua_setfield(L, -2, "A");
    lua_pushinteger(L, 1 << 1); 
    lua_setfield(L, -2, "X");
    lua_pushinteger(L, 1 << 2); 
    lua_setfield(L, -2, "Y");
    lua_pushinteger(L, 1 << 3); 
    lua_setfield(L, -2, "B");
    lua_pushinteger(L, 1 << 4); 
    lua_setfield(L, -2, "UP");
    lua_pushinteger(L, 1 << 5); 
    lua_setfield(L, -2, "DOWN");
    lua_pushinteger(L, 1 << 6); 
    lua_setfield(L, -2, "LEFT");
    lua_pushinteger(L, 1 << 7); 
    lua_setfield(L, -2, "RIGHT");
    lua_pushinteger(L, 1 << 8); 
    lua_setfield(L, -2, "LB");
    lua_pushinteger(L, 1 << 9); 
    lua_setfield(L, -2, "RB");
    lua_setglobal(L, "button");
    lua_newtable(L);
    luaL_setfuncs(L, lib_axis, 0);
    lua_pushinteger(L, 6); 
    lua_setfield(L, -2, "MAX");
    lua_pushinteger(L, 0); 
    lua_setfield(L, -2, "LEFTX");
    lua_pushinteger(L, 1); 
    lua_setfield(L, -2, "LEFTY");
    lua_pushinteger(L, 2); 
    lua_setfield(L, -2, "RIGHTX");
    lua_pushinteger(L, 3); 
    lua_setfield(L, -2, "RIGHTY");
    lua_pushinteger(L, 4); 
    lua_setfield(L, -2, "RT");
    lua_pushinteger(L, 5); 
    lua_setfield(L, -2, "LT");
    lua_setglobal(L, "axis");
    lua_newtable(L);
    luaL_setfuncs(L, lib_cursor, 0);
    lua_setglobal(L, "cursor");
    lua_newtable(L);
    luaL_setfuncs(L, lib_io, 0);
    lua_setglobal(L, "io");
    lua_newtable(L);
    luaL_setfuncs(L, lib_controller, 0);
    lua_pushinteger(L, 4); 
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "controller");
    lua_newtable(L);
    luaL_setfuncs(L, lib_name, 0);
    lua_pushinteger(L, 64); 
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "name");
    lua_newtable(L);
    luaL_setfuncs(L, lib_disk, 0);
    lua_pushinteger(L, 8); 
    lua_setfield(L, -2, "MAX");
    lua_setglobal(L, "disk");
    return NUX_SUCCESS;
}