#include "base/internal.h"
#include "nux_internal.h"

typedef struct
{
    lua_State      *L;
    nux_b32_t       serialize;
    const nux_c8_t *stack[8];
    nux_u32_t       head;
} nux_lua_serde_t;

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       value;
} nux_lua_serde_enum_t;

static void
nux_serde_begin (nux_lua_serde_t *s, lua_State *L, nux_b32_t serialize)
{
    s->L         = L;
    s->serialize = serialize;
    s->head      = 0;
    if (serialize)
    {
        lua_newtable(L);
    }
    else
    {
        NUX_ASSERT(lua_istable(L, -1));
    }
}
static void
nux_serde_field_b32 (nux_lua_serde_t *s, const nux_c8_t *name, nux_b32_t *value)
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
nux_serde_field_u32 (nux_lua_serde_t *s,
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
static nux_u32_t
nux_serde_field_enum (nux_lua_serde_t            *s,
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
nux_serde_begin_table (nux_lua_serde_t *s,
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
nux_serde_end_table (nux_lua_serde_t *s)
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

static nux_b32_t
nux_lua_get_function (lua_State *L, const nux_c8_t *func)
{
    nux_status_t status = NUX_SUCCESS;
    if (lua_getglobal(L, NUX_TABLE) != LUA_TTABLE)
    {
        lua_pop(L, 1);
        return NUX_FALSE;
    }
    if (lua_getfield(L, -1, func) != LUA_TFUNCTION)
    {
        lua_pop(L, 2); // pop field and table
        return NUX_FALSE;
    }
    lua_remove(L, -2); // remove table to let arguments on top
    return NUX_TRUE;
}
static nux_status_t
nux_lua_call (nux_ctx_t *ctx, nux_u32_t nargs, nux_u32_t nreturns)
{
    if (lua_pcall(ctx->L, nargs, nreturns, 0) != LUA_OK) // consume field
    {
        nux_error(ctx, "%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_lua_init (nux_ctx_t *ctx)
{
    // Initialize Lua VM
    ctx->L = luaL_newstate(ctx);
    NUX_ASSERT(lua_getuserdata(ctx->L) == ctx);
    NUX_ENSURE(ctx->L, return NUX_FAILURE, "failed to initialize lua state");

    // Create nux table
    lua_newtable(ctx->L);
    lua_setglobal(ctx->L, NUX_TABLE);

    // Register base lua API
    luaL_openlibs(ctx->L);

    return NUX_SUCCESS;
}
void
nux_lua_free (nux_ctx_t *ctx)
{
    if (ctx->L)
    {
        lua_close(ctx->L);
    }
}
static void
serde_config (nux_ctx_t *ctx, nux_config_t *config, nux_b32_t serialize)
{
    nux_lua_serde_t s;
    nux_serde_begin(&s, ctx->L, serialize);

    // window
    nux_serde_begin_table(&s, "window", &config->window.enable);
    nux_serde_field_u32(&s, "width", &config->window.width, 1, 8192);
    nux_serde_field_u32(&s, "height", &config->window.height, 1, 8192);
    nux_serde_end_table(&s);

    // ecs
    nux_serde_begin_table(&s, "ecs", &config->ecs.enable);
    nux_serde_end_table(&s);

    // physics
    nux_serde_begin_table(&s, "physics", &config->physics.enable);
    nux_serde_end_table(&s);

    // log
    nux_serde_begin_table(&s, "log", NUX_NULL);
    const nux_lua_serde_enum_t levels[]
        = { { .name = "debug", .value = NUX_LOG_DEBUG },
            { .name = "info", .value = NUX_LOG_INFO },
            { .name = "warning", .value = NUX_LOG_WARNING },
            { .name = "error", .value = NUX_LOG_ERROR },
            { .name = NUX_NULL, .value = 0 } };
    config->log.level
        = nux_serde_field_enum(&s, levels, "level", config->log.level);
    nux_serde_end_table(&s);

    // hotreload
    nux_serde_field_b32(&s, "hotreload", &config->hotreload);
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
nux_status_t
nux_lua_configure (nux_ctx_t      *ctx,
                   const nux_c8_t *entry_script,
                   nux_config_t   *config)
{
    // Load init script
    if (luaL_dofile(ctx->L, entry_script) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }

    // Call nux.conf
    serialize_config(ctx, config);
    if (nux_lua_get_function(ctx->L, NUX_FUNC_CONF))
    {
        lua_pushvalue(ctx->L, -2); // push config as argument
        NUX_CHECK(nux_lua_call(ctx, 1, 0), return NUX_FAILURE);
    }
    deserialize_config(ctx, config);

    return NUX_SUCCESS;
}
nux_status_t
nux_lua_call_init (nux_ctx_t *ctx)
{
    if (nux_lua_get_function(ctx->L, NUX_FUNC_INIT))
    {
        NUX_CHECK(nux_lua_call(ctx, 0, 0), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_lua_call_tick (nux_ctx_t *ctx)
{
    if (nux_lua_get_function(ctx->L, NUX_FUNC_TICK))
    {
        NUX_CHECK(nux_lua_call(ctx, 0, 0), return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_lua_dostring (nux_ctx_t *ctx, const nux_c8_t *string)
{
    if (luaL_dostring(ctx->L, string) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}

nux_res_t
nux_lua_load (nux_ctx_t *ctx, nux_res_t arena, const nux_c8_t *path)
{
    if (luaL_dofile(ctx->L, path) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_NULL;
    }
    nux_res_t  res;
    nux_lua_t *lua = nux_res_new(ctx, arena, NUX_RES_LUA, sizeof(*lua), &res);
    NUX_CHECK(lua, return NUX_NULL);
    nux_res_watch(ctx, res, path);
    return res;
}
nux_status_t
nux_lua_reload (nux_ctx_t *ctx, nux_res_t res, const nux_c8_t *path)
{
    nux_lua_t *lua = nux_res_check(ctx, NUX_RES_LUA, res);
    if (luaL_dofile(ctx->L, path) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
