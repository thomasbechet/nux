#include "internal.h"

#include "lua_api.c.inc"

nux_status_t
nux_lua_load_conf (nux_env_t *env)
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
nux_lua_init (nux_env_t *env)
{
    // Initialize Lua VM
    env->inst->L = luaL_newstate();
    NUX_CHECKM(env->inst->L, "Failed to initialize lua VM", return NUX_FAILURE);

    // Set env variable
    lua_pushlightuserdata(env->inst->L, env); // TODO: per thread env
    lua_rawseti(env->inst->L, LUA_REGISTRYINDEX, 1);

    // Load api
    luaL_openlibs(env->inst->L);
    nux_register_lua(env->inst);

    if (luaL_dofile(env->inst->L, "main.lua") != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(env->inst->L, -1));
    }

    lua_getglobal(env->inst->L, "nux");
    lua_getfield(env->inst->L, -1, "init");
    if (lua_pcall(env->inst->L, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(env->inst->L, -1));
    }
    lua_pop(env->inst->L, 1);

    return NUX_SUCCESS;
}
void
nux_lua_free (nux_env_t *env)
{
    if (env->inst->L)
    {
        lua_close(env->inst->L);
    }
}
void
nux_lua_tick (nux_env_t *env)
{
    lua_getglobal(env->inst->L, "nux");
    lua_getfield(env->inst->L, -1, "tick");
    if (lua_pcall(env->inst->L, 0, 0, 0))
    {
        NUX_ERROR("%s", lua_tostring(env->inst->L, -1));
    }
    lua_pop(env->inst->L, 1);
}
