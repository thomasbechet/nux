#include "internal.h"

#include "lua_api.c.inc"

static nux_env_t *
init_env (nux_instance_t *inst)
{
    inst->env.inst  = inst;
    inst->env.error = NUX_ERROR_NONE;
    nux_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    inst->env.arena = NUX_NULL;
    return &inst->env;
}

void
nux_set_error (nux_env_t *env, nux_error_t error)
{
    env->error = error;
    nux_snprintf(env->error_message, sizeof(env->error_message), "%s", "TODO");
}

nux_instance_t *
nux_instance_init (const nux_instance_config_t *config)
{
    NUX_ASSERT(config->max_object_count);
    NUX_ASSERT(config->memory_size);

    // Allocate instance
    nux_instance_t *inst = nux_os_malloc(
        config->userdata, NUX_MEMORY_USAGE_STATE, sizeof(struct nux_instance));
    NUX_CHECKM(inst, "Failed to allocate instance", return NUX_NULL);
    nux_memset(inst, 0, sizeof(*inst));
    inst->userdata = config->userdata;
    inst->running  = NUX_TRUE;
    inst->init     = config->init;
    inst->update   = config->update;

    // Allocate memory
    inst->arena.capa = NUX_MEMORY_SIZE;
    inst->arena.size = 0;
    inst->arena.data = nux_os_malloc(
        config->userdata, NUX_MEMORY_USAGE_STATE, inst->arena.capa);
    NUX_CHECKM(inst->arena.data, "Failed to allocate memory", return NUX_NULL);
    nux_memset(inst->arena.data, 0, inst->arena.capa);

    // Prepare initialization environment
    nux_env_t *env = init_env(inst);

    // Create object pool
    nux_object_vec_alloc(env, config->max_object_count, &inst->objects);
    nux_u32_vec_alloc(env, config->max_object_count, &inst->objects_freelist);
    nux_object_vec_push(&inst->objects); // reserve index 0 for null object
    for (nux_u32_t i = config->max_object_count - 1; i; --i)
    {
        nux_u32_vec_pushv(&inst->objects_freelist, i);
    }

    // Initialize gpu slots
    NUX_CHECKM(
        nux_u32_vec_alloc(env, NUX_GPU_TEXTURE_MAX, &inst->free_texture_slots),
        "Failed to allocate gpu texture slots",
        goto cleanup0);
    nux_u32_vec_fill_reverse_indices(&inst->free_texture_slots);

    // Allocate canvas
    inst->canvas = nux_os_malloc(config->userdata,
                                 NUX_MEMORY_USAGE_STATE,
                                 NUX_CANVAS_WIDTH * NUX_CANVAS_HEIGHT);
    NUX_CHECKM(inst->canvas, "Failed to allocate canvas", goto cleanup0);

    // Initialize state
    nux_palr(env);

    // Initialize graphics
    nux_graphics_init(inst);

    // Initialize Lua VM
    inst->L = luaL_newstate();
    NUX_CHECKM(inst->L, "Failed to initialize lua VM", goto cleanup0);

    // Set env variable
    lua_pushlightuserdata(inst->L, &inst->env);
    lua_rawseti(inst->L, LUA_REGISTRYINDEX, 1);

    // Load api
    luaL_openlibs(inst->L);
    nux_register_lua(inst);

    if (luaL_dofile(inst->L, "main.lua") != LUA_OK)
    {
        fprintf(stderr, "%s\n", lua_tostring(inst->L, -1));
        fprintf(stderr, "\n");
    }

    return inst;
cleanup0:
    nux_instance_free(inst);
    return NUX_NULL;
}
void
nux_instance_free (nux_instance_t *inst)
{
    if (inst->L)
    {
        lua_close(inst->L);
    }
    if (inst->canvas)
    {
        nux_os_free(inst->userdata, inst->canvas);
    }
    if (inst->arena.data)
    {
        nux_os_free(inst->userdata, inst->arena.data);
    }
    nux_graphics_free(inst);
    nux_os_free(inst->userdata, inst);
}
void
nux_instance_tick (nux_instance_t *inst)
{
    nux_env_t *env = init_env(inst);

    // Init draw state
    nux_cursor(env, 0, 0);

    // Init
    if (env->inst->frame == 0 && inst->init)
    {
        inst->init(env);
    }

    // Update stats
    nux_os_update_stats(env->inst->userdata, env->inst->stats);

    // Update inputs
    nux_os_update_inputs(
        env->inst->userdata, env->inst->buttons, env->inst->axis);

    // Update
    if (inst->update)
    {
        inst->update(env);
    }

    // Render
    nux_graphics_render(inst);

    // Frame integration
    inst->time += nux_dt(env);
    ++inst->frame;
}
nux_status_t
nux_instance_load (nux_instance_t *inst, const nux_c8_t *cart, nux_u32_t n)
{
    return NUX_SUCCESS;
}

void
nux_trace (nux_env_t *env, const nux_c8_t *text)
{
    nux_os_log(env->inst->userdata, text, nux_strnlen(text, 1024));
}

void
nux_dbgi32 (nux_env_t *env, const nux_c8_t *name, nux_i32_t *p)
{
    nux_os_debug(env->inst->userdata,
                 name,
                 nux_strnlen(name, NUX_NAME_MAX),
                 NUX_DEBUG_I32,
                 p);
}
void
nux_dbgf32 (nux_env_t *env, const nux_c8_t *name, nux_f32_t *p)
{
    nux_os_debug(env->inst->userdata,
                 name,
                 nux_strnlen(name, NUX_NAME_MAX),
                 NUX_DEBUG_F32,
                 p);
}

nux_u32_t
nux_stat (nux_env_t *env, nux_stat_t stat)
{
    return env->inst->stats[stat];
}
nux_f32_t
nux_time (nux_env_t *env)
{
    return env->inst->time;
}
nux_f32_t
nux_dt (nux_env_t *env)
{
    return 1. / 60;
}
nux_u32_t
nux_frame (nux_env_t *env)
{
    return env->inst->frame;
}
