#include "internal.h"

static nux_env_t *
init_env (nux_instance_t *inst)
{
    inst->env.inst  = inst;
    inst->env.error = NUX_ERROR_NONE;
    nux_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    inst->env.arena = inst->core_arena_id;
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
    nux_arena_t arena;
    arena.capa        = NUX_MEMORY_SIZE;
    arena.size        = 0;
    arena.data        = nux_os_malloc(config->userdata, NUX_MEMORY_SIZE);
    arena.last_object = NUX_NULL;
    NUX_CHECKM(arena.data, "Failed to allocate core memory", return NUX_NULL);
    nux_memset(arena.data, 0, NUX_MEMORY_SIZE);

    // Allocate instance
    nux_instance_t *inst = nux_arena_alloc(&arena, sizeof(*inst));
    NUX_ASSERT(inst);
    inst->userdata   = config->userdata;
    inst->running    = NUX_TRUE;
    inst->init       = config->init;
    inst->update     = config->update;
    inst->core_arena = arena; // copy by value

    // Register base objects
    inst->object_types_count = 0;
    nux_object_register(inst, "null", NUX_NULL);
    nux_object_register(inst, "arena", nux_arena_cleanup);
    nux_object_register(inst, "lua", NUX_NULL);
    nux_object_register(inst, "texture", nux_texture_cleanup);
    nux_object_register(inst, "mesh", NUX_NULL);
    nux_object_register(inst, "world", nux_world_cleanup);
    nux_object_register(inst, "entity", nux_entity_cleanup);
    nux_object_register(inst, "transform", NUX_NULL);
    nux_object_register(inst, "camera", NUX_NULL);

    // Create object pool
    nux_object_t *objects = nux_arena_alloc(
        &inst->core_arena, sizeof(*objects) * config->max_object_count);
    NUX_CHECKM(objects, "Failed to allocate objects pool", goto cleanup);
    nux_object_vec_init(objects, config->max_object_count, &inst->objects);
    nux_u32_t *objects_freelist
        = nux_arena_alloc(&inst->core_arena,
                          sizeof(*objects_freelist) * config->max_object_count);
    NUX_CHECKM(objects, "Failed to allocate objects freelist", goto cleanup);
    nux_u32_vec_init(
        objects_freelist, config->max_object_count, &inst->objects_freelist);

    // Reserve index 0 for null object
    nux_object_vec_push(&inst->objects);

    // Reserve index 1 for core arena
    nux_object_t *obj   = nux_object_vec_push(&inst->objects);
    obj->type           = NUX_OBJECT_ARENA;
    obj->data           = &inst->core_arena;
    obj->arena          = NUX_NULL;
    obj->prev           = NUX_NULL;
    obj->version        = 0;
    inst->core_arena_id = 1;

    return inst;
cleanup:
    nux_os_free(config->userdata, inst->core_arena.data);
    return NUX_NULL;
}

nux_instance_t *
nux_instance_init (const nux_instance_config_t *config)
{
    // Core initialization
    nux_instance_t *inst = core_init(config);

    // Prepare initialization environment
    nux_env_t *env = init_env(inst);

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
    nux_arena_set_active(env, inst->core_arena_id);
    nux_arena_reset(env);

    // Free modules
    nux_lua_free(env);
    nux_graphics_free(env);

    // Free core memory
    if (inst->core_arena.data)
    {
        nux_os_free(inst->userdata, inst->core_arena.data);
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
