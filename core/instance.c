#include "internal.h"

static nux_env_t
init_env (nux_instance_t inst)
{
    inst->env.inst  = inst;
    inst->env.error = NUX_ERROR_NONE;
    nux_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    inst->env.tricount = 0;
    return &inst->env;
}

void
nux_set_error (nux_env_t env, nux_error_t error)
{
    env->error = error;
    nux_snprintf(env->error_message,
                 sizeof(env->error_message),
                 "%s",
                 nux_error_message(error));
}

nux_instance_t
nux_instance_init (const nux_instance_config_t *config)
{
    // Allocate instance
    nux_instance_t inst = nux_platform_malloc(
        config->userdata, NUX_MEMORY_USAGE_CORE, sizeof(struct nux_instance));
    NUX_CHECK(inst, return NUX_NULL);
    nux_memset(inst, 0, sizeof(struct nux_instance));
    inst->userdata = config->userdata;
    inst->running  = NUX_TRUE;
    inst->init     = config->init;
    inst->update   = config->update;

    // Allocate state
    inst->state = nux_platform_malloc(
        config->userdata, NUX_MEMORY_USAGE_STATE, NUX_MEMORY_SIZE);
    NUX_CHECK(inst->state, goto cleanup0);
    nux_memset(inst->state, 0, NUX_MEMORY_SIZE);

    // Initialize state
    nux_env_t env = init_env(inst);
    nux_palr(env);
    nux_camfov(env, 60);
    nux_cameye(env, 0, 0, 0);
    nux_camcenter(env, 0, 0, -1);
    nux_camup(env, 0, 1, 0);
    nux_camviewport(env, 0, 0, NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);

    return inst;
cleanup0:
    nux_instance_free(inst);
    return NUX_NULL;
}
void
nux_instance_free (nux_instance_t inst)
{
    if (inst->state)
    {
        nux_platform_free(inst->userdata, inst->state);
    }
    nux_platform_free(inst->userdata, inst);
}
void
nux_instance_tick (nux_instance_t inst)
{
    nux_env_t env = init_env(inst);

    // Init draw state
    nux_cursor(env, 0, 0);

    // Init
    nux_u32_t *frame_index = (nux_u32_t *)(inst->state + NUX_RAM_FRAME);
    if (*frame_index == 0 && inst->init)
    {
        inst->init(env);
    }

    // Update
    if (inst->update)
    {
        inst->update(env);
    }

    // Frame integration
    nux_f32_t *time = (nux_f32_t *)(inst->state + NUX_RAM_TIME);
    *time           = *time + nux_dt(env);
    ++(*frame_index);
}
nux_status_t
nux_instance_load (nux_instance_t inst, const nux_c8_t *cart, nux_u32_t n)
{
    nux_env_t env = init_env(inst);
    return NUX_SUCCESS;
}
void
nux_instance_save_state (nux_instance_t inst, nux_u8_t *state)
{
}
void
nux_instance_load_state (nux_instance_t inst, const nux_u8_t *state)
{
}
void *
nux_instance_get_userdata (nux_instance_t inst)
{
    return inst->userdata;
}
void
nux_instance_set_stat (nux_instance_t inst, nux_stat_t stat, nux_u32_t value)
{
    switch (stat)
    {
        case NUX_STAT_FPS:
            NUX_MEMSET(inst, NUX_RAM_STAT_FPS, nux_u32_t, value);
            break;
    }
}
const nux_c8_t *
nux_instance_get_error (nux_instance_t inst)
{
    return nux_error_message(inst->env.error);
}
const nux_u8_t *
nux_instance_get_canvas (nux_instance_t inst)
{
    return inst->state + NUX_RAM_CANVAS;
}
const nux_u8_t *
nux_instance_get_texture (nux_instance_t inst)
{
    return inst->state + NUX_RAM_TEXTURE;
}
const nux_u8_t *
nux_instance_get_colormap (nux_instance_t inst)
{
    return inst->state + NUX_RAM_COLORMAP;
}

const nux_c8_t *
nux_error_message (nux_error_t error)
{
    switch (error)
    {
        case NUX_ERROR_NONE:
            return "none";
        case NUX_ERROR_OUT_OF_MEMORY:
            return "allocation";
        case NUX_ERROR_INVALID_TEXTURE_SIZE:
            return "invalid texture size";
        case NUX_ERROR_WASM_RUNTIME:
            return "wasm runtime";
        case NUX_ERROR_CART_EOF:
            return "cartridge EOF";
        case NUX_ERROR_CART_MOUNT:
            return "cartridge mount";
    }
    return NUX_NULL;
}

void
nux_trace (nux_env_t env, const nux_c8_t *text)
{
    nux_platform_log(env->inst, text, nux_strnlen(text, 1024));
}

void
nux_dbgi32 (nux_env_t env, const nux_c8_t *name, nux_i32_t *p)
{
    nux_platform_debug(
        env->inst, name, nux_strnlen(name, NUX_NAME_MAX), NUX_DEBUG_I32, p);
}
void
nux_dbgf32 (nux_env_t env, const nux_c8_t *name, nux_f32_t *p)
{
    nux_platform_debug(
        env->inst, name, nux_strnlen(name, NUX_NAME_MAX), NUX_DEBUG_F32, p);
}

nux_u32_t
nux_stat (nux_env_t env, nux_stat_t info)
{
    switch (info)
    {
        case NUX_STAT_FPS:
            return NUX_MEMGET(env->inst, NUX_RAM_STAT_FPS, nux_u32_t);
    }
    return 0;
}
nux_u32_t
nux_tricount (nux_env_t env)
{
    return env->tricount;
}
nux_f32_t
nux_time (nux_env_t env)
{
    return NUX_MEMGET(env->inst, NUX_RAM_TIME, nux_f32_t);
}
nux_f32_t
nux_dt (nux_env_t env)
{
    return 1. / 60;
}
nux_u32_t
nux_frame (nux_env_t env)
{
    return NUX_MEMGET(env->inst, NUX_RAM_FRAME, nux_u32_t);
}
