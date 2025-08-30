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
l_transform_get_local_translation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_get_local_translation(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
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
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_get_translation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_get_translation(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
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
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_set_translation (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t position = nux_lua_check_vec3(L, 2);
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

    nux_v3_t euler = nux_lua_check_vec3(L, 2);
    nux_transform_set_rotation_euler(ctx, e, euler);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_set_scale (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t scale = nux_lua_check_vec3(L, 2);
    nux_transform_set_scale(ctx, e, scale);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_transform_set_ortho (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t a = nux_lua_check_vec3(L, 2);

    nux_v3_t b = nux_lua_check_vec3(L, 3);

    nux_v3_t c = nux_lua_check_vec3(L, 4);
    nux_transform_set_ortho(ctx, e, a, b, c);
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
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_backward (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_backward(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_left (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_left(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_right (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_right(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_up (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_up(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_down (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t ret = nux_transform_down(ctx, e);
    l_checkerror(L, ctx);
    nux_lua_push_vec3(L, ret);
    return 1;
}
static int
l_transform_rotate (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t axis = nux_lua_check_vec3(L, 2);

    nux_f32_t angle = luaL_checknumber(L, 3);

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

    nux_v3_t center = nux_lua_check_vec3(L, 2);
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

    nux_rid_t mesh = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_staticmesh_set_mesh(ctx, e, mesh);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_staticmesh_set_texture (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_rid_t texture = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_staticmesh_set_texture(ctx, e, texture);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_staticmesh_set_colormap (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_ent_t  e   = luaL_checkinteger(L, 1);

    nux_rid_t colormap = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 2);
    nux_staticmesh_set_colormap(ctx, e, colormap);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_ecs_new_iter (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t include_count = luaL_checknumber(L, 2);

    nux_u32_t exclude_count = luaL_checknumber(L, 3);

    nux_rid_t ret = nux_ecs_new_iter(ctx, arena, include_count, exclude_count);
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
l_ecs_includes (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_rid_t  iter = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checknumber(L, 2);

    nux_ecs_includes(ctx, iter, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_excludes (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_rid_t  iter = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checknumber(L, 2);

    nux_ecs_excludes(ctx, iter, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_next (lua_State *L)
{
    nux_ctx_t *ctx  = lua_getuserdata(L);
    nux_rid_t  iter = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t e = luaL_checknumber(L, 2);

    nux_u32_t ret = nux_ecs_next(ctx, iter, e);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    nux_u32_t capa = luaL_checknumber(L, 2);

    nux_rid_t ret = nux_ecs_new(ctx, arena, capa);
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
l_ecs_load_gltf (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);

    const nux_c8_t *path = luaL_checkstring(L, 2);

    nux_rid_t ret = nux_ecs_load_gltf(ctx, arena, path);
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
l_ecs_get_active (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  ret = nux_ecs_get_active(ctx);
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
l_ecs_set_active (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_rid_t  ecs = (nux_rid_t)(nux_intptr_t)luaL_checknumber(L, 1);
    nux_ecs_set_active(ctx, ecs);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_create (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  ret = nux_ecs_create(ctx);
    l_checkerror(L, ctx);
    lua_pushinteger(L, ret);
    return 1;
}
static int
l_ecs_create_at (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checknumber(L, 1);

    nux_ecs_create_at(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_delete (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checknumber(L, 1);

    nux_ecs_delete(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_valid (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checknumber(L, 1);

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
l_ecs_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checknumber(L, 2);

    nux_ecs_remove(ctx, e, c);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_ecs_has (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_u32_t  e   = luaL_checknumber(L, 1);

    nux_u32_t c = luaL_checknumber(L, 2);

    nux_b32_t ret = nux_ecs_has(ctx, e, c);
    l_checkerror(L, ctx);
    lua_pushboolean(L, ret);
    return 1;
}

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
        { "set_ortho", l_transform_set_ortho },
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

static const struct luaL_Reg lib_ecs[] = { { "new_iter", l_ecs_new_iter },
                                           { "includes", l_ecs_includes },
                                           { "excludes", l_ecs_excludes },
                                           { "next", l_ecs_next },
                                           { "new", l_ecs_new },
                                           { "load_gltf", l_ecs_load_gltf },
                                           { "get_active", l_ecs_get_active },
                                           { "set_active", l_ecs_set_active },
                                           { "create", l_ecs_create },
                                           { "create_at", l_ecs_create_at },
                                           { "delete", l_ecs_delete },
                                           { "valid", l_ecs_valid },
                                           { "count", l_ecs_count },
                                           { "capacity", l_ecs_capacity },
                                           { "clear", l_ecs_clear },
                                           { "remove", l_ecs_remove },
                                           { "has", l_ecs_has },
                                           { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_ecs (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);

    luaL_setfuncs(L, lib_transform, 0);

    lua_setfield(L, -2, "transform"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_camera, 0);

    lua_setfield(L, -2, "camera"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_staticmesh, 0);

    lua_setfield(L, -2, "staticmesh"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_ecs, 0);

    lua_setfield(L, -2, "ecs"); // Set module to nux table

    lua_pop(L, 1);
    return NUX_SUCCESS;
}