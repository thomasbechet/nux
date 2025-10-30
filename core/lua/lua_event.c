#include "internal.h"

static void
l_checkerror (lua_State *L)
{
    if (!nux_error_get_status())
    {
        luaL_error(L, nux_error_get_message());
    }
}

static void
event_handler (void *userdata, nux_rid_t event, const void *data)
{
    lua_State   *L = nux_lua_state();
    nux_lua_t   *m = userdata;
    nux_event_t *e = nux_resource_get(NUX_RESOURCE_EVENT, event);
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
            nux_lua_call_module(m, NUX_LUA_ON_EVENT, 2);
        }
        break;
    }
}
static void
event_cleanup (void *data)
{
    nux_u32_t ref = *(nux_u32_t *)data;
    luaL_unref(nux_lua_state(), LUA_REGISTRYINDEX, ref);
}

static int
event_new (lua_State *L)
{
    nux_arena_t *arena
        = nux_resource_check(NUX_RESOURCE_ARENA, luaL_checkinteger(L, 1));
    nux_event_t *event = nux_event_new(arena, NUX_EVENT_LUA, event_cleanup);
    l_checkerror(L);
    lua_pushinteger(L, nux_resource_rid(event));
    return 1;
}
static int
event_subscribe (lua_State *L)
{
    nux_lua_t *module
        = nux_resource_check(NUX_RESOURCE_LUA_MODULE, luaL_checkinteger(L, 1));
    nux_event_t *event
        = nux_resource_check(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 2));
    l_checkerror(L);
    // check already subscribed
    for (nux_u32_t i = 0; i < module->event_handles.size; ++i)
    {
        nux_event_handler_t *handler = module->event_handles.data[i];
        if (handler->event == nux_resource_rid(event))
        {
            return 0;
        }
    }
    nux_arena_t         *arena = nux_resource_arena(module);
    nux_event_handler_t *handler
        = nux_event_subscribe(arena, event, module, event_handler);
    l_checkerror(L);
    if (!nux_ptr_vec_pushv(&module->event_handles, handler))
    {
        luaL_error(L, "failed to add handler to lua module");
    }
    return 0;
}
static int
event_unsubscribe (lua_State *L)
{
    nux_lua_t *module
        = nux_resource_check(NUX_RESOURCE_LUA_MODULE, luaL_checkinteger(L, 1));
    nux_event_t *event
        = nux_resource_check(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 2));
    l_checkerror(L);
    for (nux_u32_t i = 0; i < module->event_handles.size; ++i)
    {
        nux_event_handler_t *handler = module->event_handles.data[i];
        if (handler->event == nux_resource_rid(event))
        {
            nux_ptr_vec_swap_pop(&module->event_handles, i);
            return 0;
        }
    }
    return 0;
}
static int
event_emit (lua_State *L)
{
    nux_event_t *event
        = nux_resource_check(NUX_RESOURCE_EVENT, luaL_checkinteger(L, 1));
    // register event data
    if (lua_gettop(L) < 2)
    {
        lua_pushnil(L); // not data
    }
    nux_u32_t ref = luaL_ref(L, LUA_REGISTRYINDEX);
    nux_event_emit(event, sizeof(ref), &ref);
    return 0;
}

nux_status_t
nux_lua_open_event (void)
{
    static const struct luaL_Reg event_lib[]
        = { { "new", event_new },
            { "subscribe", event_subscribe },
            { "unsubscribe", event_unsubscribe },
            { "emit", event_emit },
            { NULL, NULL } };
    lua_State *L = nux_lua_state();
    lua_newtable(L);
    luaL_setfuncs(L, event_lib, 0);
    lua_setglobal(L, "event");
    return NUX_SUCCESS;
}
