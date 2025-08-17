#include "nux_internal.h"

static void
l_checkerror (lua_State *L, nux_ctx_t *ctx)
{
    if (!nux_error_get_status(ctx))
    {
        luaL_error(L, nux_error_get_message(ctx));
    }
}

static int
l_core_pal (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checknumber(L, 1);

    nux_u8_t color = luaL_checknumber(L, 2);

    nux_pal(ctx, index, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_palt (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u8_t   c   = luaL_checknumber(L, 1);

    nux_palt(ctx, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_palr (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_palr(ctx);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_palc (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checknumber(L, 1);

    nux_u8_t ret = nux_palc(ctx, index);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_cls (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u32_t  color = luaL_checknumber(L, 1);

    nux_cls(ctx, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_pset (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_i32_t  x   = luaL_checknumber(L, 1);

    nux_i32_t y = luaL_checknumber(L, 2);

    nux_u8_t c = luaL_checknumber(L, 3);

    nux_pset(ctx, x, y, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_cget (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checknumber(L, 1);

    nux_u32_t ret = nux_cget(ctx, index);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_cset (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checknumber(L, 2);

    nux_cset(ctx, index, c);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_texture_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_texture_type_t format = luaL_checkinteger(L, 2);

    nux_u32_t w = luaL_checknumber(L, 3);

    nux_u32_t h = luaL_checknumber(L, 4);

    nux_res_t ret = nux_texture_new(ctx, arena, format, w, h);
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
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  res = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_texture_blit(ctx, res);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_mesh_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t capa = luaL_checknumber(L, 2);

    nux_res_t ret = nux_mesh_new(ctx, arena, capa);
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
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_f32_t sx = luaL_checknumber(L, 2);

    nux_f32_t sy = luaL_checknumber(L, 3);

    nux_f32_t sz = luaL_checknumber(L, 4);

    nux_res_t ret = nux_mesh_new_cube(ctx, arena, sx, sy, sz);
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
l_mesh_gen_bounds (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_res_t  mesh = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_mesh_gen_bounds(ctx, mesh);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_canvas_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_res_t ret = nux_canvas_new(ctx, arena);
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
l_canvas_clear (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  res = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_canvas_clear(ctx, res);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_canvas_render (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  res = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_res_t target = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_canvas_render(ctx, res, target);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_canvas_text (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  res = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t x = luaL_checknumber(L, 2);

    nux_u32_t y = luaL_checknumber(L, 3);

    const nux_c8_t *text = luaL_checkstring(L, 4);
    nux_canvas_text(ctx, res, x, y, text);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_canvas_rectangle (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  res = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t x = luaL_checknumber(L, 2);

    nux_u32_t y = luaL_checknumber(L, 3);

    nux_u32_t w = luaL_checknumber(L, 4);

    nux_u32_t h = luaL_checknumber(L, 5);

    nux_canvas_rectangle(ctx, res, x, y, w, h);
    l_checkerror(L, ctx);
    return 0;
}

static const struct luaL_Reg lib_core[] = {
    { "pal", l_core_pal },   { "palt", l_core_palt }, { "palr", l_core_palr },
    { "palc", l_core_palc }, { "cls", l_core_cls },   { "pset", l_core_pset },
    { "cget", l_core_cget }, { "cset", l_core_cset }, { NUX_NULL, NUX_NULL }
};

static const struct luaL_Reg lib_texture[] = { { "new", l_texture_new },
                                               { "blit", l_texture_blit },
                                               { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_mesh[] = { { "new", l_mesh_new },
                                            { "new_cube", l_mesh_new_cube },
                                            { "gen_bounds", l_mesh_gen_bounds },
                                            { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_canvas[]
    = { { "new", l_canvas_new },
        { "clear", l_canvas_clear },
        { "render", l_canvas_render },
        { "text", l_canvas_text },
        { "rectangle", l_canvas_rectangle },
        { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_graphics (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;
    lua_getglobal(L, "nux");

    luaL_setfuncs(L, lib_core, 0);

    lua_newtable(L);
    luaL_setfuncs(L, lib_texture, 0);
    lua_setfield(L, -2, "texture");

    lua_newtable(L);
    luaL_setfuncs(L, lib_mesh, 0);
    lua_setfield(L, -2, "mesh");

    lua_newtable(L);
    luaL_setfuncs(L, lib_canvas, 0);
    lua_setfield(L, -2, "canvas");

    lua_pushinteger(L, 512);
    lua_setfield(L, -2, "CANVAS_WIDTH");

    lua_pushinteger(L, 320);
    lua_setfield(L, -2, "CANVAS_HEIGHT");

    lua_pushinteger(L, 4096);
    lua_setfield(L, -2, "TEXTURE_WIDTH");

    lua_pushinteger(L, 4096);
    lua_setfield(L, -2, "TEXTURE_HEIGHT");

    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "PALETTE_SIZE");

    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "COLORMAP_SIZE");

    lua_pushinteger(L, 1024);
    lua_setfield(L, -2, "GPU_COMMAND_SIZE");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "PRIMITIVE_TRIANGLES");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "PRIMITIVE_LINES");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "PRIMITIVE_POINTS");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "VERTEX_TRIANGLES");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "VERTEX_LINES");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "VERTEX_POINTS");

    lua_pushinteger(L, 1 << 0);
    lua_setfield(L, -2, "VERTEX_POSITION");

    lua_pushinteger(L, 1 << 1);
    lua_setfield(L, -2, "VERTEX_UV");

    lua_pushinteger(L, 1 << 2);
    lua_setfield(L, -2, "VERTEX_COLOR");

    lua_pushinteger(L, 1 << 3);
    lua_setfield(L, -2, "VERTEX_INDICES");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "V3F");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "V3F_T2F");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "V3F_T2F_N3F");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TRIANGLES");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "LINES");

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "TEXTURE_IMAGE_RGBA");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "TEXTURE_IMAGE_INDEX");

    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "TEXTURE_RENDER_TARGET");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}