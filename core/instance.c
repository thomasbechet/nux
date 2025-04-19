#include "internal.h"

static nux_f32_t
delta_time (nux_instance_t inst)
{
    return 1.0 / (nu_f32_t)inst->tps;
}
static nux_env_t
init_env (nux_instance_t inst)
{
    inst->env.inst  = inst;
    inst->env.error = NUX_ERROR_NONE;
    nu_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    return &inst->env;
}

void
nux_set_error (nux_env_t env, nux_error_t error)
{
    env->error = error;
    nu_snprintf(env->error_message,
                sizeof(env->error_message),
                "%s",
                nux_error_message(error));
}

nux_instance_t
nux_instance_init (const nux_instance_config_t *config)
{
    // Instance allocation
    nux_instance_t inst = nux_platform_malloc(
        config->userdata, NUX_MEMORY_USAGE_CORE, sizeof(struct nux_instance));
    NU_CHECK(inst, return NU_NULL);
    nu_memset(inst, 0, sizeof(struct nux_instance));
    inst->userdata = config->userdata;
    inst->running  = NU_TRUE;
    inst->tps      = 60;
    inst->init     = config->init;
    inst->update   = config->update;

    // State allocation
    inst->memory_capa = NU_MEM_16M;
    inst->memory      = nux_platform_malloc(
        config->userdata, NUX_MEMORY_USAGE_STATE, inst->memory_capa);
    NU_CHECK(inst->memory, goto cleanup0);
    nu_memset(inst->memory, 0, inst->memory_capa);

    // Wasm initialization
    // nux_status_t res = nux_wasm_init(inst, config);
    // NU_CHECK(res, goto cleanup0);

    return inst;
cleanup0:
    nux_instance_free(inst);
    return NU_NULL;
}
void
nux_instance_free (nux_instance_t inst)
{
    // Wasm
    // nux_wasm_free(inst);
    // State
    if (inst->memory)
    {
        nux_platform_free(inst->userdata, inst->memory);
    }
    // Instance
    nux_platform_free(inst->userdata, inst);
}
void
nux_instance_tick (nux_instance_t inst)
{
    nux_env_t env = init_env(inst);

    // Init draw state
    nux_cursor(env, 0, 0);

    // Init
    nux_u32_t *frame_index = (nux_u32_t *)(inst->memory + NUX_MAP_FRAME);
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
    nux_f32_t *time = (nux_f32_t *)(inst->memory + NUX_MAP_TIME);
    *time           = *time + delta_time(inst);
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
const nux_c8_t *
nux_instance_get_error (nux_instance_t inst)
{
    return nux_error_message(inst->env.error);
}
const nux_u8_t *
nux_instance_get_screen (nux_instance_t inst)
{
    return inst->memory + NUX_MAP_SCREEN;
}
const nux_u8_t *
nux_instance_get_palette (nux_instance_t inst)
{
    return inst->memory + NUX_MAP_PALETTE;
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
    return NU_NULL;
}

void
nux_trace (nux_env_t env, const nux_c8_t *text)
{
    nux_platform_log(env->inst, text, nu_strnlen(text, 1024));
}

void
nux_dbgi32 (nux_env_t env, const nux_c8_t *name, nux_i32_t *p)
{
    nux_platform_debug(
        env->inst, name, nu_strnlen(name, NUX_NAME_MAX), NUX_DEBUG_I32, p);
}
void
nux_dbgf32 (nux_env_t env, const nux_c8_t *name, nux_f32_t *p)
{
    nux_platform_debug(
        env->inst, name, nu_strnlen(name, NUX_NAME_MAX), NUX_DEBUG_F32, p);
}

nux_u32_t
nux_stat (nux_env_t env, nux_console_info_t info)
{
    return 0;
}
nux_f32_t
nux_gtime (nux_env_t env)
{
    return NUX_MEMGET(env, NUX_MAP_TIME, nux_f32_t);
}
nux_f32_t
nux_dtime (nux_env_t env)
{
    return delta_time(env->inst);
}
nux_u32_t
nux_frame (nux_env_t env)
{
    return NUX_MEMGET(env, NUX_MAP_FRAME, nux_u32_t);
}
