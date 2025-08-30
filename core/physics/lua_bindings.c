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
l_rigidbody_add (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_eid_t  e   = luaL_checkinteger(L, 1);

    nux_rigidbody_add(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_rigidbody_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_eid_t  e   = luaL_checkinteger(L, 1);

    nux_rigidbody_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_rigidbody_set_velocity (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_eid_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t velocity = nux_lua_check_vec3(L, 2);
    nux_rigidbody_set_velocity(ctx, e, velocity);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_collider_add_sphere (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_eid_t  e   = luaL_checkinteger(L, 1);

    nux_f32_t radius = luaL_checknumber(L, 2);

    nux_collider_add_sphere(ctx, e, radius);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_collider_add_aabb (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_eid_t  e   = luaL_checkinteger(L, 1);

    nux_v3_t min = nux_lua_check_vec3(L, 2);

    nux_v3_t max = nux_lua_check_vec3(L, 3);
    nux_collider_add_aabb(ctx, e, min, max);
    l_checkerror(L, ctx);
    return 0;
}
static int
l_collider_remove (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_eid_t  e   = luaL_checkinteger(L, 1);

    nux_collider_remove(ctx, e);
    l_checkerror(L, ctx);
    return 0;
}

static int
l_physics_raycast (lua_State *L)
{
    nux_ctx_t *ctx = lua_getuserdata(L);
    nux_v3_t   pos = nux_lua_check_vec3(L, 1);

    nux_v3_t dir = nux_lua_check_vec3(L, 2);

    nux_raycast_hit_t ret = nux_physics_raycast(ctx, pos, dir);
    l_checkerror(L, ctx);
    if (ret.entity)
    {
        nux_lua_push_hit(L, ret);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

static const struct luaL_Reg lib_rigidbody[]
    = { { "add", l_rigidbody_add },
        { "remove", l_rigidbody_remove },
        { "set_velocity", l_rigidbody_set_velocity },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_collider[]
    = { { "add_sphere", l_collider_add_sphere },
        { "add_aabb", l_collider_add_aabb },
        { "remove", l_collider_remove },
        { NUX_NULL, NUX_NULL } };

static const struct luaL_Reg lib_physics[]
    = { { "raycast", l_physics_raycast }, { NUX_NULL, NUX_NULL } };

nux_status_t
nux_lua_open_physics (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_getglobal(L, "nux");

    lua_newtable(L);

    luaL_setfuncs(L, lib_rigidbody, 0);

    lua_setfield(L, -2, "rigidbody"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_collider, 0);

    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "SPHERE");

    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "AABB");

    lua_setfield(L, -2, "collider"); // Set module to nux table

    lua_newtable(L);

    luaL_setfuncs(L, lib_physics, 0);

    lua_setfield(L, -2, "physics"); // Set module to nux table

    lua_pop(L, 1);
    return NUX_SUCCESS;
}