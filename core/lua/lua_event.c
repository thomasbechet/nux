#include "internal.h"

static void
l_checkerror (lua_State *L, nux_ctx_t *ctx)
{
    if (!nux_error_get_status(ctx))
    {
        luaL_error(L, nux_error_get_message(ctx));
    }
}

static void
event_handler (nux_ctx_t  *ctx,
               void       *userdata,
               nux_rid_t   event,
               const void *data)
{
    lua_State          *L = ctx->lua->L;
    nux_lua_vmmodule_t *m = userdata;
    nux_event_t        *e = nux_resource_get(ctx, NUX_RESOURCE_EVENT, event);
    NUX_ASSERT(e);
    switch (e->type)
    {
        case NUX_EVENT_LUA: {
            // push event
            lua_pushinteger(L, event);
            // push data
            nux_u32_t ref = *(nux_u32_t *)data;
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
            // call on_event
            nux_lua_call_module(ctx, m->rid, NUX_FUNC_EVENT, 2);
        }
        break;
    }
}

static int
event_new (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  arena = luaL_checkinteger(L, 1);
    nux_rid_t  event = nux_event_new(ctx, arena, NUX_EVENT_LUA);
    l_checkerror(L, ctx);
    lua_pushinteger(L, event);
    return 1;
}
static int
event_subscribe (lua_State *L)
{
    nux_ctx_t          *ctx    = lua_getuserdata(L);
    nux_rid_t           module = luaL_checkinteger(L, 1);
    nux_rid_t           event  = luaL_checkinteger(L, 2);
    nux_lua_vmmodule_t *m
        = nux_resource_check(ctx, NUX_RESOURCE_LUA_MODULE, module);
    l_checkerror(L, ctx);
    // check already subscribed
    for (nux_u32_t i = 0; i < m->event_handles.size; ++i)
    {
        nux_event_handler_t *handler = m->event_handles.data[i];
        if (handler->event == event)
        {
            return 0;
        }
    }
    nux_rid_t            arena = nux_resource_get_arena(ctx, module);
    nux_event_handler_t *handler
        = nux_event_subscribe(ctx, arena, event, m, event_handler);
    l_checkerror(L, ctx);
    if (!nux_ptr_vec_pushv(&m->event_handles, handler))
    {
        luaL_error(L, "failed to add handler to lua module");
    }
    return 0;
}
static int
event_unsubscribe (lua_State *L)
{
    nux_ctx_t          *ctx    = lua_getuserdata(L);
    nux_rid_t           module = luaL_checkinteger(L, 1);
    nux_rid_t           event  = luaL_checkinteger(L, 2);
    nux_lua_vmmodule_t *m
        = nux_resource_check(ctx, NUX_RESOURCE_LUA_MODULE, module);
    l_checkerror(L, ctx);
    for (nux_u32_t i = 0; i < m->event_handles.size; ++i)
    {
        nux_event_handler_t *handler = m->event_handles.data[i];
        if (handler->event == event)
        {
            nux_ptr_vec_swap_pop(&m->event_handles, i);
            return 0;
        }
    }
    return 0;
}
static int
event_emit (lua_State *L)
{
    nux_ctx_t *ctx   = lua_getuserdata(L);
    nux_rid_t  event = luaL_checkinteger(L, 1);
    // register event data
    if (lua_gettop(L) < 2)
    {
        lua_pushnil(L); // not data
    }
    nux_u32_t ref = luaL_ref(L, LUA_REGISTRYINDEX);
    nux_event_emit(ctx, event, sizeof(ref), &ref);
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
