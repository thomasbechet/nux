#include "internal.h"

#include "lua_code.c.inc"

#include <io/module.h>

static struct
{
    lua_State *L;
} _module;

static void
serde_begin (nux_lua_serde_t *s, lua_State *L, nux_b32_t serialize)
{
    s->L         = L;
    s->serialize = serialize;
    s->head      = 0;
    NUX_ASSERT(lua_istable(L, -1));
}
static void
serde_field_b32 (nux_lua_serde_t *s, const nux_c8_t *name, nux_b32_t *value)
{
    if (s->serialize)
    {
        NUX_ASSERT(lua_istable(s->L, -1));
        lua_pushboolean(s->L, *value);
        lua_setfield(s->L, -2, name);
    }
    else
    {
        if (lua_istable(s->L, -1))
        {
            if (lua_getfield(s->L, -1, name) == LUA_TBOOLEAN)
            {
                *value = lua_toboolean(s->L, -1);
            }
        }
        lua_pop(s->L, 1);
    }
}
static void
serde_field_u32_minmax (nux_lua_serde_t *s,
                        const nux_c8_t  *name,
                        nux_u32_t       *value,
                        nux_u32_t        min,
                        nux_u32_t        max)
{
    if (s->serialize)
    {
        NUX_ASSERT(lua_istable(s->L, -1));
        lua_pushinteger(s->L, *value);
        lua_setfield(s->L, -2, name);
    }
    else
    {
        if (lua_istable(s->L, -1))
        {
            if (lua_getfield(s->L, -1, name) == LUA_TNUMBER)
            {
                *value = NUX_CLAMP(lua_tointeger(s->L, -1), min, max);
            }
        }
        lua_pop(s->L, 1);
    }
}
static void
serde_field_u32 (nux_lua_serde_t *s, const nux_c8_t *name, nux_u32_t *value)
{
    return serde_field_u32_minmax(s, name, value, NUX_U32_MIN, NUX_U32_MAX);
}
static nux_u32_t
serde_field_enum (nux_lua_serde_t            *s,
                  const nux_lua_serde_enum_t *enums,
                  const nux_c8_t             *name,
                  nux_u32_t                   value)
{
    if (s->serialize)
    {
        NUX_ASSERT(lua_istable(s->L, -1));

        const nux_lua_serde_enum_t *e = enums;
        while (e->name)
        {
            if (e->value == value)
            {
                lua_pushstring(s->L, e->name);
                lua_setfield(s->L, -2, name);
                return value;
            }
            ++e;
        }
        NUX_ASSERT(NUX_FALSE);
        return 0;
    }
    else
    {
        nux_u32_t ret = value;
        if (lua_istable(s->L, -1))
        {
            if (lua_getfield(s->L, -1, name) == LUA_TSTRING)
            {
                const nux_c8_t             *str = lua_tostring(s->L, -1);
                const nux_lua_serde_enum_t *e   = enums;
                while (e->name)
                {
                    if (nux_strncmp(e->name, str, nux_strnlen(e->name, 64))
                        == 0)
                    {
                        ret = e->value;
                        break;
                    }
                    ++e;
                }
            }
        }
        lua_pop(s->L, 1);
        return ret;
    }
}
static void
serde_begin_table (nux_lua_serde_t *s,
                   const nux_c8_t  *name,
                   nux_b32_t       *has_table)
{
    NUX_ASSERT(s->head < NUX_ARRAY_SIZE(s->stack));
    if (s->serialize)
    {
        NUX_ASSERT(lua_istable(s->L, -1));
        lua_newtable(s->L);
    }
    else
    {
        if (lua_istable(s->L, -1))
        {
            lua_getfield(s->L, -1, name);
        }
        else
        {
            lua_pushnil(s->L);
        }
        if (has_table)
        {
            *has_table = lua_istable(s->L, -1);
        }
    }
    s->stack[s->head] = name;
    ++s->head;
}
static void
serde_end_table (nux_lua_serde_t *s)
{
    NUX_ASSERT(s->head);
    --s->head;
    if (s->serialize)
    {
        NUX_ASSERT(lua_istable(s->L, -1));
        lua_setfield(s->L, -2, s->stack[s->head]);
    }
    else
    {
        lua_pop(s->L, 1);
    }
}
nux_status_t
nux_lua_call_function (nux_u32_t nargs, nux_u32_t nreturns)
{
    if (lua_pcall(_module.L, nargs, nreturns, 0) != LUA_OK) // consume field
    {
        nux_error("%s", lua_tostring(_module.L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_lua_call_module (nux_lua_t *lua, const nux_c8_t *name, nux_u32_t nargs)
{
    nux_status_t status = NUX_SUCCESS;
    lua_State   *L      = _module.L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
    NUX_ASSERT(lua_istable(L, -1));
    // -1=M
    lua_getfield(L, -1, name);
    // -2=M -1=F
    if (lua_isfunction(L, -1))
    {
        lua_insert(L, -2 - nargs); // move function before args
        lua_insert(L, -1 - nargs); // move module before args
        status = nux_lua_call_function(1 + nargs, 0);
    }
    else
    {
        lua_pop(L, 2 + nargs); // remove M + F + args
    }
    return status;
}
static void
serde_config (nux_config_t *config, nux_b32_t serialize)
{
    nux_lua_serde_t s;
    serde_begin(&s, _module.L, serialize);

    // window
    serde_begin_table(&s, "window", &config->window.enable);
    serde_field_u32(&s, "width", &config->window.width);
    serde_field_u32(&s, "height", &config->window.height);
    serde_end_table(&s);

    // scene
    serde_begin_table(&s, "scene", &config->scene.enable);
    serde_end_table(&s);

    // physics
    serde_begin_table(&s, "physics", &config->physics.enable);
    serde_end_table(&s);

    // log
    serde_begin_table(&s, "log", NUX_NULL);
    const nux_lua_serde_enum_t levels[]
        = { { .name = "debug", .value = NUX_LOG_DEBUG },
            { .name = "info", .value = NUX_LOG_INFO },
            { .name = "warning", .value = NUX_LOG_WARNING },
            { .name = "error", .value = NUX_LOG_ERROR },
            { .name = NUX_NULL, .value = 0 } };
    config->log.level
        = serde_field_enum(&s, levels, "level", config->log.level);
    serde_end_table(&s);

    // hotreload
    serde_field_b32(&s, "hotreload", &config->hotreload);

    // graphics
    serde_begin_table(&s, "graphics", NUX_NULL);
    serde_field_u32(
        &s, "batches_buffer_size", &config->graphics.batches_buffer_size);
    serde_field_u32(
        &s, "transforms_buffer_size", &config->graphics.transforms_buffer_size);
    serde_field_u32(
        &s, "vertices_buffer_size", &config->graphics.vertices_buffer_size);
    serde_end_table(&s);

    // debug
    serde_begin_table(&s, "debug", &config->debug.enable);
    serde_field_b32(&s, "console", &config->debug.console);
    serde_end_table(&s);
}
static void
serialize_config (nux_config_t *config)
{
    serde_config(config, NUX_TRUE);
}
static void
deserialize_config (nux_config_t *config)
{
    serde_config(config, NUX_FALSE);
}

static void
copy_functions (lua_State *L, int src, int dst)
{
    lua_pushnil(L);
    while (lua_next(L, src) != 0)
    {
        // -2 = k, -1 = v
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            // -3 = k, -2 = v, -1 = k
            lua_insert(L, -2);
            // -3 = k, -2 = k, -1 = v
            lua_rawset(L, dst);
            // -1 = k
        }
        else
        {
            lua_pop(L, 1);
            // -1 = k
        }
    }
}
static nux_status_t
load_lua_module (nux_lua_t *lua, const nux_c8_t *path)
{
    lua_State *L = _module.L;

    // 1. keep previous module on stack
    lua_getglobal(L, NUX_LUA_MODULE_TABLE);

    // 2. set global MODULE
    if (lua->ref)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
        luaL_unref(L, LUA_REGISTRYINDEX, lua->ref);
    }
    else
    {
        lua_newtable(L);
        lua_pushinteger(L, nux_resource_rid(lua));
        lua_setfield(L, -2, NUX_LUA_MODULE_RID);
    }
    NUX_ASSERT(lua_istable(L, -1));
    lua_setglobal(L, NUX_LUA_MODULE_TABLE);

    // 3. execute module
    nux_u32_t prev = lua_gettop(L);
    if (luaL_dofile(L, path) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(L, -1));
        return NUX_FAILURE;
    }

    // 4. assign module table to registry
    nux_u32_t nret = lua_gettop(L) - prev;
    if (nret)
    {
        NUX_ENSURE(nret == 1 && lua_istable(L, -1),
                   return NUX_FAILURE,
                   "lua module '%s' returned value is not a table",
                   path);
    }
    else
    {
        lua_getglobal(L, NUX_LUA_MODULE_TABLE);
        NUX_ENSURE(lua_istable(L, -1),
                   return NUX_FAILURE,
                   "lua module table '%s' removed",
                   path);
    }
    lua->ref = luaL_ref(L, LUA_REGISTRYINDEX);

    // 5. reset previous MODULE global
    lua_setglobal(L, NUX_LUA_MODULE_TABLE); // reset previous module

    return NUX_SUCCESS;
}
nux_lua_t *
nux_lua_load (nux_arena_t *arena, const nux_c8_t *path)
{
    nux_lua_t *lua = nux_resource_new(arena, NUX_RESOURCE_LUA_MODULE);
    NUX_CHECK(lua, return NUX_NULL);
    nux_resource_set_path(lua, path);
    // initialize event handles
    nux_ptr_vec_init(nux_arena_allocator(arena), &lua->event_handles);
    // dofile and call load function
    NUX_CHECK(load_lua_module(lua, path), return NUX_NULL);
    NUX_CHECK(nux_lua_call_module(lua, NUX_LUA_ON_LOAD, 0), return NUX_NULL);
    return lua;
}
static void
lua_module_cleanup (void *data)
{
    nux_lua_t *lua = data;
    // unregister lua module
    lua_State *L = _module.L;
    luaL_unref(L, LUA_REGISTRYINDEX, lua->ref);
    // unsubscribe events
    for (nux_u32_t i = 0; i < lua->event_handles.size; ++i)
    {
        const nux_event_handler_t *handler = lua->event_handles.data[i];
        nux_event_unsubscribe(handler);
    }
}
static nux_status_t
lua_module_reload (void *data, const nux_c8_t *path)
{
    nux_lua_t *lua = data;
    NUX_CHECK(load_lua_module(lua, path), return NUX_FAILURE);
    NUX_CHECK(nux_lua_call_module(lua, NUX_LUA_ON_RELOAD, 0),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}

static nux_status_t
module_init (void)
{
    // Register types
    nux_resource_type_t *type;
    type = nux_resource_register(
        NUX_RESOURCE_LUA_MODULE, sizeof(nux_lua_t), "lua_module");
    type->cleanup = lua_module_cleanup;
    type->reload  = lua_module_reload;

    // Initialize Lua VM
    _module.L = luaL_newstate();
    NUX_ENSURE(_module.L, return NUX_FAILURE, "failed to initialize lua state");

    // Register base lua API
    luaL_openlibs(_module.L);

    // Register lua API
    nux_lua_open_api();
    nux_lua_open_require();
    nux_lua_open_vmath();
    nux_lua_open_event();
    nux_lua_dostring(lua_data_code);

    return NUX_SUCCESS;
}
static nux_status_t
module_free (void)
{
    if (_module.L)
    {
        lua_close(_module.L);
    }
    return NUX_SUCCESS;
}
static nux_status_t
module_update (void)
{
    nux_lua_t *lua = NUX_NULL;
    while ((lua = nux_resource_next(NUX_RESOURCE_LUA_MODULE, lua)))
    {
        nux_lua_call_module(lua, NUX_LUA_ON_UPDATE, 0);
    }
    return NUX_SUCCESS;
}
const nux_module_info_t *
nux_lua_module_info (void)
{
    static const nux_module_info_t info = {
        .name   = "lua",
        .size   = sizeof(_module),
        .data   = &_module,
        .init   = module_init,
        .free   = module_free,
        .update = module_update,
    };
    return &info;
}
lua_State *
nux_lua_state (void)
{
    return _module.L;
}

nux_status_t
nux_lua_configure (nux_config_t *config)
{
    if (nux_file_exists(NUX_LUA_CONF_FILE))
    {
        // Execute configuration script
        if (luaL_dofile(_module.L, NUX_LUA_CONF_FILE) != LUA_OK)
        {
            NUX_ERROR("%s", lua_tostring(_module.L, -1));
            return NUX_FAILURE;
        }

        // Call nux.conf
        lua_newtable(_module.L);
        serialize_config(config);

        if (lua_getglobal(_module.L, NUX_LUA_ON_CONF) != LUA_TFUNCTION)
        {
            lua_pop(_module.L, 1);
            return NUX_FAILURE;
        }
        lua_pushvalue(_module.L, -2); // push config as argument
        NUX_CHECK(nux_lua_call_function(1, 0), return NUX_FAILURE);

        deserialize_config(config);
        lua_pop(_module.L, 1);
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_lua_dostring (const nux_c8_t *string)
{
    if (luaL_dostring(_module.L, string) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(_module.L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
