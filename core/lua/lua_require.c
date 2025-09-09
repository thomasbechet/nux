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
l_require (lua_State *L)
{
    nux_ctx_t      *ctx  = lua_getuserdata(L);
    const nux_c8_t *path = luaL_checkstring(L, 1);
    nux_c8_t        filepath[NUX_PATH_BUF_SIZE];
    nux_path_normalize(filepath, path);
    nux_path_set_extension(filepath, "lua");

    // check already loaded
    nux_rid_t rid = NUX_NULL;
    while ((rid = nux_resource_next(ctx, NUX_RESOURCE_LUA, rid)))
    {
        if (!nux_strncmp(nux_resource_get_path(ctx, rid), filepath, NUX_PATH_MAX))
        {
            nux_lua_t *lua = nux_resource_check(ctx, NUX_RESOURCE_LUA, rid);
            NUX_ASSERT(lua);
            lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
            return 1;
        }
    }

    // load the module
    nux_rid_t module = nux_lua_load(ctx, nux_arena_core(ctx), filepath);
    l_checkerror(L, ctx);
    nux_lua_t *lua = nux_resource_check(ctx, NUX_RESOURCE_LUA, module);
    NUX_ASSERT(lua);
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
    return 1;
}

nux_status_t
nux_lua_open_require (nux_ctx_t *ctx)
{
    lua_State *L = ctx->lua->L;
    lua_pushcfunction(L, l_require);
    lua_setglobal(L, "require");
    return NUX_SUCCESS;
}
