#include "internal.h"

static nux_env_t *
init_env (nux_instance_t *inst)
{
    inst->env.inst  = inst;
    inst->env.error = NUX_ERROR_NONE;
    nux_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    inst->env.active_arena = inst->core_arena;
    return &inst->env;
}

void
nux_error (nux_env_t *env, nux_error_t error)
{
    env->error = error;
    nux_snprintf(env->error_message, sizeof(env->error_message), "%s", "TODO");
}

static nux_instance_t *
core_init (const nux_instance_config_t *config)
{
    NUX_ASSERT(config->max_object_count);
    NUX_ASSERT(config->memory_size);

    // Allocate core memory
    nux_arena_t core_arena;
    core_arena.capa = NUX_CORE_MEMORY_SIZE;
    core_arena.size = 0;
    core_arena.data = nux_os_malloc(config->userdata, NUX_CORE_MEMORY_SIZE);
    core_arena.first_object = NUX_NULL;
    core_arena.last_object  = NUX_NULL;
    NUX_CHECK(core_arena.data, return NUX_NULL);
    nux_memset(core_arena.data, 0, NUX_CORE_MEMORY_SIZE);

    // Allocate instance
    nux_instance_t *inst = nux_arena_alloc(&core_arena, sizeof(*inst));
    NUX_ASSERT(inst);
    inst->userdata = config->userdata;
    inst->running  = NUX_TRUE;
    inst->init     = config->init;
    inst->update   = config->update;

    // Register base objects
    inst->object_types_count = 0;
    nux_object_register(inst, "null", NUX_NULL);
    nux_object_register(inst, "arena", NUX_NULL);
    nux_object_register(inst, "lua", NUX_NULL);
    nux_object_register(inst, "texture", nux_texture_cleanup);
    nux_object_register(inst, "mesh", NUX_NULL);
    nux_object_register(inst, "scene", nux_scene_cleanup);
    nux_object_register(inst, "entity", NUX_NULL);
    nux_object_register(inst, "transform", NUX_NULL);
    nux_object_register(inst, "camera", NUX_NULL);

    // Create object pool
    NUX_CHECK(nux_object_pool_alloc(
                  &core_arena, config->max_object_count, &inst->objects),
              goto cleanup);

    // Reserve index 0 for null object
    nux_object_pool_add(&inst->objects);

    // Register core arena
    NUX_CHECK(nux_arena_pool_alloc(&core_arena, 32, &inst->arenas),
              goto cleanup);
    inst->core_arena  = nux_arena_pool_add(&inst->arenas);
    *inst->core_arena = core_arena; // copy by value

    return inst;
cleanup:
    nux_os_free(config->userdata, inst->core_arena->data);
    return NUX_NULL;
}

nux_instance_t *
nux_instance_init (const nux_instance_config_t *config)
{
    // Core initialization
    nux_instance_t *inst = core_init(config);

    // Prepare initialization environment
    nux_env_t *env = init_env(inst);

    // Register core arena object
    inst->core_arena->id = nux_object_create(
        env, env->active_arena, NUX_OBJECT_ARENA, inst->core_arena);

    // Load configuration
    NUX_CHECK(nux_lua_load_conf(env), goto cleanup);

    // Initialize modules
    NUX_CHECK(nux_graphics_init(env), goto cleanup);
    NUX_CHECK(nux_lua_init(env), goto cleanup);

    return inst;

cleanup:
    nux_instance_free(inst);
    return NUX_NULL;
}
void
nux_instance_free (nux_instance_t *inst)
{
    nux_env_t *env = init_env(inst);
    nux_arena_reset_to(env, inst->core_arena, NUX_NULL);

    // Free modules
    nux_lua_free(env);
    nux_graphics_free(env);

    // Free core memory
    if (inst->core_arena->data)
    {
        nux_os_free(inst->userdata, inst->core_arena->data);
    }
}
void
nux_instance_tick (nux_instance_t *inst)
{
    nux_env_t *env = init_env(inst);

    // Init draw state
    nux_graphics_cursor(env, 0, 0);

    // Init
    if (env->inst->frame == 0 && inst->init)
    {
        inst->init(env);
    }

    // Update stats
    nux_os_update_stats(env->inst->userdata, env->inst->stats);

    // Keep previous input state
    nux_input_pre_update(env);

    // Update inputs
    nux_os_update_inputs(
        env->inst->userdata, env->inst->buttons, env->inst->axis);

    // Update
    if (inst->update)
    {
        inst->update(env);
    }

    // Update lua
    nux_lua_tick(env);

    // Render
    nux_graphics_render(env);

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
    nux_log(env, NUX_LOG_INFO, text, nux_strnlen(text, 1024));
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
