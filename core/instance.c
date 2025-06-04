#include "internal.h"

#include "lua_api.c.inc"

static nux_env_t *
init_env (nux_instance_t *inst)
{
    inst->env.inst  = inst;
    inst->env.error = NUX_ERROR_NONE;
    nux_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    inst->env.tricount = 0;
    return &inst->env;
}

void *
nux_malloc (nux_env_t *env, nux_u32_t size)
{
    return NUX_VEC_PUSHN(&env->inst->memory, size);
}
void
nux_set_error (nux_env_t *env, nux_error_t error)
{
    env->error = error;
    nux_snprintf(env->error_message,
                 sizeof(env->error_message),
                 "%s",
                 nux_error_message(error));
}
const nux_c8_t *
nux_error_message (nux_error_t error)
{
    switch (error)
    {
        case NUX_ERROR_NONE:
        case NUX_ERROR_OUT_OF_MEMORY:
        case NUX_ERROR_INVALID_TEXTURE_SIZE:
        case NUX_ERROR_WASM_RUNTIME:
        case NUX_ERROR_CART_EOF:
        case NUX_ERROR_CART_MOUNT:
            return "error";
    }
    return NUX_NULL;
}
void *
nux_add_object (nux_env_t *env, nux_object_type_t type, nux_u32_t *id)
{
    nux_u32_t  index;
    nux_u32_t *free = NUX_VEC_POP(&env->inst->free_objects);
    if (free)
    {
        index = *free;
    }
    else
    {
        index = env->inst->objects.size;
        if (NUX_VEC_PUSH(&env->inst->objects) == NUX_NULL)
        {
            return NUX_NULL;
        }
    }
    env->inst->objects.data[index].type = type;
    *id                                 = index;
    return &env->inst->objects.data[index].data;
}
nux_status_t
nux_remove_object (nux_env_t *env, nux_u32_t id)
{
    if (id <= env->inst->objects.size
        || env->inst->objects.data[id].type == NUX_OBJECT_NULL)
    {
        return NUX_FAILURE;
    }
    env->inst->objects.data[id].type        = NUX_OBJECT_NULL;
    *NUX_VEC_PUSH(&env->inst->free_objects) = id;
    return NUX_SUCCESS;
}
void *
nux_check_object (nux_env_t *env, nux_u32_t id, nux_object_type_t type)
{
    if (id <= env->inst->objects.size
        || env->inst->objects.data[id].type != type)
    {
        return NUX_NULL;
    }
    return &env->inst->objects.data[id].data;
}

nux_instance_t *
nux_instance_init (const nux_instance_config_t *config)
{
    NUX_ASSERT(config->max_object_count);
    NUX_ASSERT(config->memory_size);

    // Allocate instance
    nux_instance_t *inst = nux_os_malloc(
        config->userdata, NUX_MEMORY_USAGE_CORE, sizeof(struct nux_instance));
    NUX_CHECK(inst, "Failed to allocate instance core memory", return NUX_NULL);
    nux_memset(inst, 0, sizeof(struct nux_instance));
    inst->userdata = config->userdata;
    inst->running  = NUX_TRUE;
    inst->init     = config->init;
    inst->update   = config->update;

    // Allocate memory
    void *p = nux_os_malloc(
        config->userdata, NUX_MEMORY_USAGE_STATE, NUX_MEMORY_SIZE);
    NUX_CHECK(p, "Failed to allocate instance state memory", goto cleanup0);
    nux_memset(p, 0, NUX_MEMORY_SIZE);
    NUX_VEC_INIT(&inst->memory, p, NUX_MEMORY_SIZE);

    // Allocate objects pool
    p = nux_os_malloc(config->userdata,
                      NUX_MEMORY_USAGE_STATE,
                      sizeof(*inst->objects.data) * config->max_object_count);
    NUX_CHECK(p, "Failed to allocate objects pool", goto cleanup0);
    NUX_VEC_INIT(&inst->objects, p, config->max_object_count);
    p = nux_os_malloc(config->userdata,
                      NUX_MEMORY_USAGE_STATE,
                      sizeof(*inst->free_objects.data)
                          * config->max_object_count);
    NUX_CHECK(p, "Failed to allocate objects pool freelist", goto cleanup0);
    NUX_VEC_INIT(&inst->free_objects, p, config->max_object_count);
    for (nux_u32_t i = 0; i < inst->objects.capa; ++i)
    {
        inst->objects.data[i].type = NUX_OBJECT_NULL;
    }

    // Allocate canvas
    inst->canvas = nux_os_malloc(config->userdata,
                                 NUX_MEMORY_USAGE_CORE,
                                 NUX_CANVAS_WIDTH * NUX_CANVAS_HEIGHT);
    NUX_CHECK(inst->canvas, "Failed to allocate canvas", goto cleanup0);

    // Initialize state
    nux_env_t *env = init_env(inst);
    nux_palr(env);
    nux_camfov(env, 60);
    nux_cameye(env, 0, 0, 0);
    nux_camcenter(env, 0, 0, -1);
    nux_camup(env, 0, 1, 0);
    nux_camviewport(env, 0, 0, NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);

    // Initialize graphics
    nux_graphics_init(inst);

    // Initialize Lua VM
    inst->L = luaL_newstate();
    NUX_CHECK(inst->L, "Failed to initialize lua VM", goto cleanup0);

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
    if (inst->free_objects.data)
    {
        nux_os_free(inst->userdata, inst->free_objects.data);
    }
    if (inst->objects.data)
    {
        nux_os_free(inst->userdata, inst->objects.data);
    }
    if (inst->memory.data)
    {
        nux_os_free(inst->userdata, inst->memory.data);
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
nux_u32_t
nux_tricount (nux_env_t *env)
{
    return env->tricount;
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
