#include "nux_internal.h"

static void
l_checkerror (lua_State *L, nux_ctx_t *ctx)
{
    if (!nux_error_get_status(ctx))
    {
        luaL_error(L, nux_error_get_message(ctx));
    }
}

nux_status_t
nux_lua_open_physics (nux_ctx_t *ctx)
{
    lua_State *L = ctx->L;
    lua_getglobal(L, "nux");

    lua_pop(L, 1);
    return NUX_SUCCESS;
}