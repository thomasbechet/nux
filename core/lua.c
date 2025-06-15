#include "internal.h"

#include "lua_api.c.inc"

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
        fprintf(stderr, "%s\n", lua_tostring(env->inst->L, -1));
        fprintf(stderr, "\n");
    }

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
}
