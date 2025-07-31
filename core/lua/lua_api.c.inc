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
l_core_trace (lua_State *L)
{
    nux_ctx_t      *ctx  = lua_getuserdata(L);
    const nux_c8_t *text = luaL_checkstring(L, 1);
    nux_trace(ctx, text);
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
l_core_pal (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u8_t color = luaL_checkinteger(L, 2);

    nux_pal(ctx, index, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_palt (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u8_t   c   = luaL_checkinteger(L, 1);

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
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u8_t ret = nux_palc(ctx, index);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_cls (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u32_t  color = luaL_checkinteger(L, 1);

    nux_cls(ctx, color);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_pset (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_i32_t  x   = luaL_checkinteger(L, 1);

    nux_i32_t y = luaL_checkinteger(L, 2);

    nux_u8_t c = luaL_checkinteger(L, 3);

    nux_pset(ctx, x, y, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_core_cget (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u32_t ret = nux_cget(ctx, index);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_core_cset (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_u8_t   index = luaL_checkinteger(L, 1);

    nux_u32_t c = luaL_checkinteger(L, 2);

    nux_cset(ctx, index, c);
    l_checkerror(L, ctx);
    return 0;
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
l_texture_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_texture_type_t format = luaL_checkinteger(L, 2);

    nux_u32_t w = luaL_checkinteger(L, 3);

    nux_u32_t h = luaL_checkinteger(L, 4);

    nux_res_t ret = nux_texture_new(ctx, arena, format, w, h);
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
l_texture_blit (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  res = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_texture_blit(ctx, res);
    l_checkerror(L, ctx);
    return 0;
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
l_transform_add (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_transform_add(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_transform_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_set_parent (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_ent_t parent = luaL_checkinteger(L, 2);

    nux_transform_set_parent(ctx, e, parent);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_get_parent (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_ent_t ret = nux_transform_get_parent(ctx, e);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_transform_get_local_translation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_get_local_translation(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_get_local_rotation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_q4_t ret = nux_transform_get_local_rotation(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    lua_pushnumber(L, ret.w);
    return 4;
}
static int
l_transform_get_local_scale (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_get_local_scale(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_get_translation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_get_translation(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_get_rotation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_q4_t ret = nux_transform_get_rotation(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    lua_pushnumber(L, ret.w);
    return 4;
}
static int
l_transform_get_scale (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_get_scale(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_set_translation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t position;
    position.x = luaL_checknumber(L, 2);
    position.y = luaL_checknumber(L, 3);
    position.z = luaL_checknumber(L, 4);
    nux_transform_set_translation(ctx, e, position);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_set_rotation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_q4_t rotation;
    rotation.x = luaL_checknumber(L, 2);
    rotation.y = luaL_checknumber(L, 3);
    rotation.z = luaL_checknumber(L, 4);
    rotation.w = luaL_checknumber(L, 5);
    nux_transform_set_rotation(ctx, e, rotation);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_set_rotation_euler (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t euler;
    euler.x = luaL_checknumber(L, 2);
    euler.y = luaL_checknumber(L, 3);
    euler.z = luaL_checknumber(L, 4);
    nux_transform_set_rotation_euler(ctx, e, euler);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_set_scale (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t scale;
    scale.x = luaL_checknumber(L, 2);
    scale.y = luaL_checknumber(L, 3);
    scale.z = luaL_checknumber(L, 4);
    nux_transform_set_scale(ctx, e, scale);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_forward (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_forward(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_backward (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_backward(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_left (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_left(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_right (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_right(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_up (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_up(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_down (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_down(ctx, e);
    l_checkerror(L, ctx);
    lua_pushnumber(L, ret.x);
    lua_pushnumber(L, ret.y);
    lua_pushnumber(L, ret.z);
    return 3;
}
static int
l_transform_rotate (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t axis;
    axis.x = luaL_checknumber(L, 2);
    axis.y = luaL_checknumber(L, 3);
    axis.z = luaL_checknumber(L, 4);

    nux_f32_t angle = luaL_checknumber(L, 5);

    nux_transform_rotate(ctx, e, axis, angle);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_rotate_x (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_x(ctx, e, angle);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_rotate_y (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_y(ctx, e, angle);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_rotate_z (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t angle = luaL_checknumber(L, 2);

    nux_transform_rotate_z(ctx, e, angle);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_look_at (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t center;
    center.x = luaL_checknumber(L, 2);
    center.y = luaL_checknumber(L, 3);
    center.z = luaL_checknumber(L, 4);
    nux_transform_look_at(ctx, e, center);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_camera_add (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_camera_add(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_camera_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_camera_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_camera_set_fov (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t fov = luaL_checknumber(L, 2);

    nux_camera_set_fov(ctx, e, fov);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_camera_set_near (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t near = luaL_checknumber(L, 2);

    nux_camera_set_near(ctx, e, near);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_camera_set_far (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t far = luaL_checknumber(L, 2);

    nux_camera_set_far(ctx, e, far);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_staticmesh_add (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_staticmesh_add(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_staticmesh_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_staticmesh_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_staticmesh_set_mesh (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_res_t mesh = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_staticmesh_set_mesh(ctx, e, mesh);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_staticmesh_set_texture (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_res_t texture = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_staticmesh_set_texture(ctx, e, texture);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_staticmesh_set_colormap (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_res_t colormap = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_staticmesh_set_colormap(ctx, e, colormap);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_mesh_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t capa = luaL_checkinteger(L, 2);

    nux_res_t ret = nux_mesh_new(ctx, arena, capa);
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
        lua_pushnumber(L, (nux_intptr_t)ret);
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
        lua_pushnumber(L, (nux_intptr_t)ret);
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

    nux_u32_t x = luaL_checkinteger(L, 2);

    nux_u32_t y = luaL_checkinteger(L, 3);

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

    nux_u32_t x = luaL_checkinteger(L, 2);

    nux_u32_t y = luaL_checkinteger(L, 3);

    nux_u32_t w = luaL_checkinteger(L, 4);

    nux_u32_t h = luaL_checkinteger(L, 5);

    nux_canvas_rectangle(ctx, res, x, y, w, h);
    l_checkerror(L, ctx);
    return 0;
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

static int
l_ecs_new_iter (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t include_count = luaL_checkinteger(L, 2);

    nux_u32_t exclude_count = luaL_checkinteger(L, 3);

    nux_res_t ret = nux_ecs_new_iter(ctx, arena, include_count, exclude_count);
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
l_ecs_includes (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_res_t  iter = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checkinteger(L, 2);

    nux_ecs_includes(ctx, iter, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_excludes (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_res_t  iter = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checkinteger(L, 2);

    nux_ecs_excludes(ctx, iter, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_begin (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_res_t  iter = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t ret = nux_ecs_begin(ctx, iter);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_next (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_res_t  iter = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t ret = nux_ecs_next(ctx, iter);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t capa = luaL_checkinteger(L, 2);

    nux_res_t ret = nux_ecs_new(ctx, arena, capa);
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
l_ecs_load_gltf (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_res_t  arena = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);

    const nux_c8_t *path = luaL_checkstring(L, 2);

    nux_res_t ret = nux_ecs_load_gltf(ctx, arena, path);
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
l_ecs_get_active (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  ret = nux_ecs_get_active(ctx);
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
l_ecs_set_active (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_res_t  ecs = (nux_res_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_ecs_set_active(ctx, ecs);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_render (lua_State *L)
{
    nux_ctx_t *ctx    = lua_getuserdata(L);
    nux_ent_t  camera = luaL_checkinteger(L, 1);

    nux_ecs_render(ctx, camera);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_add (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_ecs_add(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_add_at (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checkinteger(L, 1);

    nux_ecs_add_at(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checkinteger(L, 1);

    nux_ecs_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_valid (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checkinteger(L, 1);

    nux_b32_t ret = nux_ecs_valid(ctx, e);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}
static int
l_ecs_count (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_ecs_count(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_capacity (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_ecs_capacity(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_clear (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ecs_clear(ctx);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_unset (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checkinteger(L, 1);

    nux_u32_t c = luaL_checkinteger(L, 2);

    nux_ecs_unset(ctx, e, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_has (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checkinteger(L, 1);

    nux_u32_t c = luaL_checkinteger(L, 2);

    nux_b32_t ret = nux_ecs_has(ctx, e, c);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}

static const struct luaL_Reg lib_core[]
    = { { "trace", l_core_trace }, { "stat", l_core_stat },
        { "time", l_core_time },   { "dt", l_core_dt },
        { "frame", l_core_frame }, { "random", l_core_random },
        { "pal", l_core_pal },     { "palt", l_core_palt },
        { "palr", l_core_palr },   { "palc", l_core_palc },
        { "cls", l_core_cls },     { "pset", l_core_pset },
        { "cget", l_core_cget },   { "cset", l_core_cset },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_log[]
    = { { "set_level", l_log_set_level }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_input[] = { { "button", l_input_button },
                                             { "axis", l_input_axis },
                                             { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_button[]
    = { { "pressed", l_button_pressed },
        { "released", l_button_released },
        { "just_pressed", l_button_just_pressed },
        { "just_released", l_button_just_released },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_texture[] = { { "new", l_texture_new },
                                               { "blit", l_texture_blit },
                                               { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_arena[] = { { "new", l_arena_new },
                                             { "reset", l_arena_reset },
                                             { "frame", l_arena_frame },
                                             { "scratch", l_arena_scratch },
                                             { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_transform[]
    = { { "add", l_transform_add },
        { "remove", l_transform_remove },
        { "set_parent", l_transform_set_parent },
        { "get_parent", l_transform_get_parent },
        { "get_local_translation", l_transform_get_local_translation },
        { "get_local_rotation", l_transform_get_local_rotation },
        { "get_local_scale", l_transform_get_local_scale },
        { "get_translation", l_transform_get_translation },
        { "get_rotation", l_transform_get_rotation },
        { "get_scale", l_transform_get_scale },
        { "set_translation", l_transform_set_translation },
        { "set_rotation", l_transform_set_rotation },
        { "set_rotation_euler", l_transform_set_rotation_euler },
        { "set_scale", l_transform_set_scale },
        { "forward", l_transform_forward },
        { "backward", l_transform_backward },
        { "left", l_transform_left },
        { "right", l_transform_right },
        { "up", l_transform_up },
        { "down", l_transform_down },
        { "rotate", l_transform_rotate },
        { "rotate_x", l_transform_rotate_x },
        { "rotate_y", l_transform_rotate_y },
        { "rotate_z", l_transform_rotate_z },
        { "look_at", l_transform_look_at },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_camera[]
    = { { "add", l_camera_add },         { "remove", l_camera_remove },
        { "set_fov", l_camera_set_fov }, { "set_near", l_camera_set_near },
        { "set_far", l_camera_set_far }, { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_staticmesh[]
    = { { "add", l_staticmesh_add },
        { "remove", l_staticmesh_remove },
        { "set_mesh", l_staticmesh_set_mesh },
        { "set_texture", l_staticmesh_set_texture },
        { "set_colormap", l_staticmesh_set_colormap },
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

static const struct luaL_Reg lib_io[]
    = { { "cart_begin", l_io_cart_begin },
        { "cart_end", l_io_cart_end },
        { "write_cart_file", l_io_write_cart_file },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_ecs[] = { { "new_iter", l_ecs_new_iter },
                                           { "includes", l_ecs_includes },
                                           { "excludes", l_ecs_excludes },
                                           { "begin", l_ecs_begin },
                                           { "next", l_ecs_next },
                                           { "new", l_ecs_new },
                                           { "load_gltf", l_ecs_load_gltf },
                                           { "get_active", l_ecs_get_active },
                                           { "set_active", l_ecs_set_active },
                                           { "render", l_ecs_render },
                                           { "add", l_ecs_add },
                                           { "add_at", l_ecs_add_at },
                                           { "remove", l_ecs_remove },
                                           { "valid", l_ecs_valid },
                                           { "count", l_ecs_count },
                                           { "capacity", l_ecs_capacity },
                                           { "clear", l_ecs_clear },
                                           { "unset", l_ecs_unset },
                                           { "has", l_ecs_has },
                                           { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_register_base (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;
    lua_getglobal(L, "nux");

    luaL_setfuncs(L, lib_core, 0);

    lua_newtable(L);
    luaL_setfuncs(L, lib_log, 0);
    lua_setfield(L, -2, "log");

    lua_newtable(L);
    luaL_setfuncs(L, lib_input, 0);
    lua_setfield(L, -2, "input");

    lua_newtable(L);
    luaL_setfuncs(L, lib_button, 0);
    lua_setfield(L, -2, "button");

    lua_newtable(L);
    luaL_setfuncs(L, lib_texture, 0);
    lua_setfield(L, -2, "texture");

    lua_newtable(L);
    luaL_setfuncs(L, lib_arena, 0);
    lua_setfield(L, -2, "arena");

    lua_newtable(L);
    luaL_setfuncs(L, lib_transform, 0);
    lua_setfield(L, -2, "transform");

    lua_newtable(L);
    luaL_setfuncs(L, lib_camera, 0);
    lua_setfield(L, -2, "camera");

    lua_newtable(L);
    luaL_setfuncs(L, lib_staticmesh, 0);
    lua_setfield(L, -2, "staticmesh");

    lua_newtable(L);
    luaL_setfuncs(L, lib_mesh, 0);
    lua_setfield(L, -2, "mesh");

    lua_newtable(L);
    luaL_setfuncs(L, lib_canvas, 0);
    lua_setfield(L, -2, "canvas");

    lua_newtable(L);
    luaL_setfuncs(L, lib_io, 0);
    lua_setfield(L, -2, "io");

    lua_newtable(L);
    luaL_setfuncs(L, lib_ecs, 0);
    lua_setfield(L, -2, "ecs");

    lua_pushinteger(L, 512);
    lua_setfield(L, -2, "CANVAS_WIDTH");

    lua_pushinteger(L, 320);
    lua_setfield(L, -2, "CANVAS_HEIGHT");

    lua_pushinteger(L, 4096);
    lua_setfield(L, -2, "TEXTURE_WIDTH");

    lua_pushinteger(L, 4096);
    lua_setfield(L, -2, "TEXTURE_HEIGHT");

    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "PLAYER_MAX");

    lua_pushinteger(L, 10);
    lua_setfield(L, -2, "BUTTON_MAX");

    lua_pushinteger(L, 6);
    lua_setfield(L, -2, "AXIS_MAX");

    lua_pushinteger(L, 64);
    lua_setfield(L, -2, "NAME_MAX");

    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "PALETTE_SIZE");

    lua_pushinteger(L, 256);
    lua_setfield(L, -2, "COLORMAP_SIZE");

    lua_pushinteger(L, 1024);
    lua_setfield(L, -2, "GPU_COMMAND_SIZE");

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