#include "internal.h"

static void
event_handler (nux_ctx_t  *ctx,
               void       *userdata,
               nux_rid_t   event,
               const void *data)
{
    nux_lua_script_t *lua = userdata;
}

static int
event_new (lua_State *L)
{
    return 0;
}
static int
event_subscribe (lua_State *L)
{
    return 0;
}
static int
event_unsubscribe (lua_State *L)
{
    return 0;
}
static int
event_emit (lua_State *L)
{
    return 0;
}

nux_status_t
nux_lua_open_event (nux_ctx_t *ctx)
{
    static const struct luaL_Reg event_lib[]
        = { { "new", event_new },
            { "subscribe", event_subscribe },
            { "unsubscribe", event_unsubscribe },
            { "emit", event_emit },
            { NULL, NULL } };
    lua_State *L = ctx->lua->L;
    lua_newtable(L);
    luaL_setfuncs(L, event_lib, 0);
    lua_setglobal(L, "event");
    return NUX_SUCCESS;
}
