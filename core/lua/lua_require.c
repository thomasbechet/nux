#include "internal.h"

static void
l_checkerror (lua_State *L)
{
    if (!nux_error_get_status())
    {
        luaL_error(L, nux_error_get_message());
    }
}

static int
l_require (lua_State *L)
{
    const nux_c8_t *path = luaL_checkstring(L, 1);
    nux_c8_t        filepath[NUX_PATH_BUF_SIZE];
    nux_path_normalize(filepath, path);
    nux_path_set_extension(filepath, "lua");

    // check already loaded
    nux_rid_t rid = NUX_NULL;
    while ((rid = nux_resource_next(NUX_RESOURCE_LUA_MODULE, rid)))
    {
        if (!nux_strncmp(nux_resource_path(rid), filepath, NUX_PATH_MAX))
        {
            nux_lua_t *lua = nux_resource_check(NUX_RESOURCE_LUA_MODULE, rid);
            NUX_ASSERT(lua);
            lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
            return 1;
        }
    }

    // load the module
    nux_lua_t *lua = nux_lua_load(nux_arena_core(), filepath);
    l_checkerror(L);
    NUX_ASSERT(lua);
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
    return 1;
}

nux_status_t
nux_lua_open_require (void)
{
    lua_State *L = nux_lua_module()->L;
    lua_pushcfunction(L, l_require);
    lua_setglobal(L, "require");
    return NUX_SUCCESS;
}
