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
l_texture_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_texture_type_t format = luaL_checkinteger(L, 2);

    nux_u32_t w = luaL_checknumber(L, 3);

    nux_u32_t h = luaL_checknumber(L, 4);

    nux_rid_t ret = nux_texture_new(ctx, arena, format, w, h);
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
l_texture_blit (lua_State *L)
{
    nux_ctx_t *ctx     = lua_getuserdata(L);
    nux_rid_t  texture = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_texture_blit(ctx, texture);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_mesh_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t capa = luaL_checknumber(L, 2);

    nux_rid_t ret = nux_mesh_new(ctx, arena, capa);
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
l_mesh_new_cube (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_f32_t sx = luaL_checknumber(L, 2);

    nux_f32_t sy = luaL_checknumber(L, 3);

    nux_f32_t sz = luaL_checknumber(L, 4);

    nux_rid_t ret = nux_mesh_new_cube(ctx, arena, sx, sy, sz);
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
l_mesh_update_bounds (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_rid_t  mesh = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_mesh_update_bounds(ctx, mesh);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_mesh_bounds_min (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_rid_t  mesh = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_v3_t ret = nux_mesh_bounds_min(ctx, mesh);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_mesh_bounds_max (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_rid_t  mesh = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_v3_t ret = nux_mesh_bounds_max(ctx, mesh);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}

static int
l_canvas_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t width = luaL_checknumber(L, 2);

    nux_u32_t height = luaL_checknumber(L, 3);

    nux_rid_t ret = nux_canvas_new(ctx, arena, width, height);
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
l_canvas_get_texture (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  rid = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_rid_t ret = nux_canvas_get_texture(ctx, rid);
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
l_canvas_set_layer (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  rid = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_i32_t layer = luaL_checknumber(L, 2);

    nux_canvas_set_layer(ctx, rid, layer);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_canvas_set_clear_color (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  rid = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t color = luaL_checknumber(L, 2);

    nux_canvas_set_clear_color(ctx, rid, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_canvas_text (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  rid = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t x = luaL_checknumber(L, 2);

    nux_u32_t y = luaL_checknumber(L, 3);

    const nux_c8_t *text = luaL_checkstring(L, 4);
    nux_canvas_text(ctx, rid, x, y, text);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_canvas_rectangle (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  rid = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t x = luaL_checknumber(L, 2);

    nux_u32_t y = luaL_checknumber(L, 3);

    nux_u32_t w = luaL_checknumber(L, 4);

    nux_u32_t h = luaL_checknumber(L, 5);

    nux_canvas_rectangle(ctx, rid, x, y, w, h);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_graphics_draw_line_tr (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_m4_t   tr  = nux_lua_check_mat4(L, 1);

    nux_v3_t a = nux_lua_check_vec3(L, 2);

    nux_v3_t b = nux_lua_check_vec3(L, 3);

    nux_u32_t color = luaL_checknumber(L, 4);

    nux_graphics_draw_line_tr(ctx, tr, a, b, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_graphics_draw_line (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_v3_t   a   = nux_lua_check_vec3(L, 1);

    nux_v3_t b = nux_lua_check_vec3(L, 2);

    nux_u32_t color = luaL_checknumber(L, 3);

    nux_graphics_draw_line(ctx, a, b, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_graphics_draw_dir (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_v3_t   origin = nux_lua_check_vec3(L, 1);

    nux_v3_t dir = nux_lua_check_vec3(L, 2);

    nux_f32_t length = luaL_checknumber(L, 3);

    nux_u32_t color = luaL_checknumber(L, 4);

    nux_graphics_draw_dir(ctx, origin, dir, length, color);
    l_checkerror(L, ctx);
    return 0;
}

static const struct luaL_Reg lib_texture[] = { { "new", l_texture_new },
                                               { "blit", l_texture_blit },
                                               { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_mesh[]
    = { { "new", l_mesh_new },
        { "new_cube", l_mesh_new_cube },
        { "update_bounds", l_mesh_update_bounds },
        { "bounds_min", l_mesh_bounds_min },
        { "bounds_max", l_mesh_bounds_max },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_canvas[]
    = { { "new", l_canvas_new },
        { "get_texture", l_canvas_get_texture },
        { "set_layer", l_canvas_set_layer },
        { "set_clear_color", l_canvas_set_clear_color },
        { "text", l_canvas_text },
        { "rectangle", l_canvas_rectangle },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_graphics[]
    = { { "draw_line_tr", l_graphics_draw_line_tr },
        { "draw_line", l_graphics_draw_line },
        { "draw_dir", l_graphics_draw_dir },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_palette[] = { { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_colormap[] = { { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_primitive[] = { { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_vertex[] = { { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_graphics (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);

    luaL_setfuncs(L, lib_texture, 0);

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "IMAGE_RGBA");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "IMAGE_INDEX");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "RENDER_TARGET");

    lua_setfield(L, -2, "texture"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_mesh, 0);

    lua_setfield(L, -2, "mesh"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_canvas, 0);

    lua_pushinteger(L, 640);
    lua_setfield(L, -2, "WIDTH");

    lua_pushinteger(L, 400);
    lua_setfield(L, -2, "HEIGHT");

    lua_setfield(L, -2, "canvas"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_graphics, 0);

    lua_setfield(L, -2, "graphics"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_palette, 0);

    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "SIZE");

    lua_setfield(L, -2, "palette"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_colormap, 0);

    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "SIZE");

    lua_setfield(L, -2, "colormap"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_primitive, 0);

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TRIANGLES");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LINES");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "POINTS");

    lua_setfield(L, -2, "primitive"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_vertex, 0);

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TRIANGLES");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LINES");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "POINTS");

    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "POSITION");

    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "UV");

    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "COLOR");

    lua_pushinteger(L, 1 << 3);
    lua_setfield(L, -2, "INDICES");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "V3F");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "V3F_T2F");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "V3F_T2F_N3F");

    lua_setfield(L, -2, "vertex"); // Set module to nux table

    lua_pop(L, 1);
    return NUX_SUCCESS;
}