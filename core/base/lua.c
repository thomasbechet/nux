#include "nux.h"

#include "lua_code.c.inc"

typedef struct
{
    lua_State      *L;
    nux_b32_t       serialize;
    const nux_c8_t *stack[8];
    nux_u32_t       head;
} nux_lua_serde_t;

typedef enum
{
    NUX_SERDE_SERIALIZE,
} nux_lua_serde_mode_t;

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
nux_serde_end (nux_lua_serde_t *s)
{
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

    // arena
    nux_serde_begin_table(&s, "arena", NUX_NULL);
    nux_serde_field_u32(
        &s, "global_capacity", &config->arena.global_capacity, 0, NUX_MEM_2G);
    nux_serde_field_u32(
        &s, "frame_capacity", &config->arena.frame_capacity, 0, NUX_MEM_2G);
    nux_serde_field_u32(
        &s, "scratch_capacity", &config->arena.scratch_capacity, 0, NUX_MEM_2G);
    nux_serde_end_table(&s);

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

    // hotreload
    nux_serde_field_b32(&s, "hotreload", &config->hotreload);

    nux_serde_end(&s);
}
nux_status_t
nux_lua_configure (nux_ctx_t      *ctx,
                   const nux_c8_t *entry_script,
                   nux_config_t   *config)
{
    // Load init script
    nux_res_t res = nux_lua_load(ctx, ctx->core_arena.self, entry_script);
    NUX_CHECK(res, return NUX_FAILURE);

    // Call nux.conf
    serde_config(ctx, config, NUX_TRUE); // serialize config
    if (nux_lua_get_function(ctx->L, NUX_FUNC_CONF))
    {
        lua_pushvalue(ctx->L, -2); // push config as argument
        NUX_CHECK(nux_lua_call(ctx, 1, 0), return NUX_FAILURE);
    }
    serde_config(ctx, config, NUX_FALSE); // deserialize config

    // Register file to os for hotreload (before configuration)
    if (config->hotreload)
    {
        const nux_c8_t *path
            = ((nux_lua_t *)nux_res_check(ctx, NUX_RES_LUA, res))->path;
        nux_os_hotreload_add(ctx->userdata, path, res);
    }

    // Register base API
    nux_lua_open_base(ctx);
    nux_lua_dostring(ctx, lua_data_code);

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
    nux_lua_t *lua
        = nux_arena_alloc_res(ctx, arena, NUX_RES_LUA, sizeof(*lua), &res);
    NUX_CHECK(lua, return NUX_NULL);
    lua->path = nux_arena_alloc_path(ctx, arena, path);
    NUX_CHECK(lua->path, return NUX_NULL);
    if (ctx->config.hotreload)
    {
        nux_os_hotreload_add(ctx->userdata, lua->path, res);
    }
    return res;
}
nux_status_t
nux_lua_hotreload (nux_ctx_t *ctx, void *data)
{
    nux_lua_t *lua = data;
    NUX_ASSERT(lua->path);
    if (luaL_dofile(ctx->L, lua->path) != LUA_OK)
    {
        NUX_ERROR("%s", lua_tostring(ctx->L, -1));
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
