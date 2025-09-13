#include "internal.h"

#include "lua_code.c.inc"

#include <io/internal.h>

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
static nux_status_t
lua_call_function (nux_ctx_t *ctx, nux_u32_t nargs, nux_u32_t nreturns)
{
    nux_lua_module_t *module = ctx->lua;
    if (lua_pcall(module->L, nargs, nreturns, 0) != LUA_OK) // consume field
    {
        nux_error(ctx, "%s", lua_tostring(module->L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
static void
serde_config (nux_ctx_t *ctx, nux_config_t *config, nux_b32_t serialize)
{
    nux_lua_module_t *module = ctx->lua;

    nux_lua_serde_t s;
    serde_begin(&s, module->L, serialize);

    // window
    serde_begin_table(&s, "window", &config->window.enable);
    serde_field_u32(&s, "width", &config->window.width);
    serde_field_u32(&s, "height", &config->window.height);
    serde_end_table(&s);

    // ecs
    serde_begin_table(&s, "ecs", &config->ecs.enable);
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
serialize_config (nux_ctx_t *ctx, nux_config_t *config)
{
    serde_config(ctx, config, NUX_TRUE);
}
static void
deserialize_config (nux_ctx_t *ctx, nux_config_t *config)
{
    serde_config(ctx, config, NUX_FALSE);
}

void
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
load_lua_module (nux_ctx_t *ctx, nux_rid_t rid, const nux_c8_t *path)
{
    nux_lua_t *lua = nux_resource_check(ctx, NUX_RESOURCE_LUA, rid);
    lua_State *L   = ctx->lua->L;

    // 1. keep previous module on stack
    lua_getglobal(L, NUX_MODULE_TABLE);

    // 2. set global MODULE
    if (lua->ref)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
        luaL_unref(L, LUA_REGISTRYINDEX, lua->ref);
    }
    else
    {
        lua_newtable(L);
    }
    NUX_ASSERT(lua_istable(L, -1));
    lua_setglobal(L, NUX_MODULE_TABLE);

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
        lua_getglobal(L, NUX_MODULE_TABLE);
        NUX_ENSURE(lua_istable(L, -1),
                   return NUX_FAILURE,
                   "lua module table '%s' removed",
                   path);
    }
    lua->ref = luaL_ref(L, LUA_REGISTRYINDEX);

    // 5. reset previous MODULE global
    lua_setglobal(L, NUX_MODULE_TABLE); // reset previous module

    return NUX_SUCCESS;
}
nux_rid_t
nux_lua_load (nux_ctx_t *ctx, nux_rid_t arena, const nux_c8_t *path)
{
    nux_rid_t  rid;
    nux_lua_t *lua = nux_resource_new(ctx, arena, NUX_RESOURCE_LUA, &rid);
    NUX_CHECK(lua, return NUX_NULL);
    nux_resource_set_path(ctx, rid, path);
    NUX_CHECK(load_lua_module(ctx, rid, path), return NUX_NULL);
    NUX_CHECK(nux_lua_call_module(ctx, rid, NUX_FUNC_LOAD), return NUX_NULL);
    return rid;
}
void
nux_lua_cleanup (nux_ctx_t *ctx, nux_rid_t rid)
{
    // Remove lua module from loaded
    nux_lua_t *lua = nux_resource_check(ctx, NUX_RESOURCE_LUA, rid);
    lua_State *L   = ctx->lua->L;
    luaL_unref(L, LUA_REGISTRYINDEX, lua->ref);
}
nux_status_t
nux_lua_reload (nux_ctx_t *ctx, nux_rid_t rid, const nux_c8_t *path)
{
    NUX_CHECK(load_lua_module(ctx, rid, path), return NUX_FAILURE);
    NUX_CHECK(nux_lua_call_module(ctx, rid, NUX_FUNC_RELOAD),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}

nux_status_t
nux_lua_init (nux_ctx_t *ctx)
{
    ctx->lua = nux_arena_malloc(&ctx->core_arena, sizeof(*ctx->lua));
    NUX_CHECK(ctx->lua, return NUX_FAILURE);

    nux_lua_module_t *module = ctx->lua;

    // Register types
    nux_resource_type_t *type;
    type = nux_resource_register(
        ctx, NUX_RESOURCE_LUA, sizeof(nux_lua_t), "lua");
    type->cleanup = nux_lua_cleanup;
    type->reload  = nux_lua_reload;

    // Initialize Lua VM
    module->L = luaL_newstate(ctx);
    NUX_ASSERT(lua_getuserdata(module->L) == ctx);
    NUX_ENSURE(module->L, return NUX_FAILURE, "failed to initialize lua state");

    // Register base lua API
    luaL_openlibs(module->L);

    // Register lua API
    nux_lua_open_api(ctx);
    nux_lua_open_require(ctx);
    nux_lua_open_vmath(ctx);
    nux_lua_dostring(ctx, lua_data_code);

    return NUX_SUCCESS;
}
void
nux_lua_free (nux_ctx_t *ctx)
{
    nux_lua_module_t *module = ctx->lua;
    NUX_CHECK(module, return);

    if (module->L)
    {
        lua_close(module->L);
    }
}
nux_status_t
nux_lua_configure (nux_ctx_t *ctx, nux_config_t *config)
{
    nux_lua_module_t *module = ctx->lua;

    if (nux_io_exists(ctx, NUX_CONF_FILE))
    {
        // Execute configuration script
        if (luaL_dofile(module->L, NUX_CONF_FILE) != LUA_OK)
        {
            NUX_ERROR("%s", lua_tostring(module->L, -1));
            return NUX_FAILURE;
        }

        // Call nux.conf
        lua_newtable(module->L);
        serialize_config(ctx, config);

        if (lua_getglobal(module->L, NUX_FUNC_CONF) != LUA_TFUNCTION)
        {
            lua_pop(module->L, 1);
            return NUX_FAILURE;
        }
        lua_pushvalue(module->L, -2); // push config as argument
        NUX_CHECK(lua_call_function(ctx, 1, 0), return NUX_FAILURE);

        deserialize_config(ctx, config);
        lua_pop(module->L, 1);
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_lua_call_module (nux_ctx_t *ctx, nux_rid_t module, const nux_c8_t *name)
{
    nux_status_t status = NUX_SUCCESS;
    lua_State   *L      = ctx->lua->L;
    nux_lua_t   *lua    = nux_resource_check(ctx, NUX_RESOURCE_LUA, module);
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua->ref);
    NUX_ASSERT(lua_istable(L, -1));
    lua_getfield(L, -1, name);
    if (lua_isfunction(L, -1))
    {
        lua_pushvalue(L, -2); // push self
        status = lua_call_function(ctx, 1, 0);
    }
    else
    {
        lua_pop(L, 1);
    }
    lua_pop(L, 1); // pop table
    return status;
}
nux_status_t
nux_lua_dostring (nux_ctx_t *ctx, const nux_c8_t *string)
{
    if (luaL_dostring(ctx->lua->L, string) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->lua->L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
