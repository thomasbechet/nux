#include "internal.h"

static nux_f32_t
delta_time (nux_instance_t inst)
{
    return 1.0 / (nu_f32_t)inst->tps;
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
    inst->mem      = NU_NULL;
    inst->memhead  = 0;
    inst->memcapa  = NU_MEM_16M;
    inst->userdata = config->userdata;

    // State allocation
    inst->mem = nux_platform_malloc(
        config->userdata, NUX_MEMORY_USAGE_STATE, NU_MEM_16M);
    NU_CHECK(inst->mem, goto cleanup0);
    nu_memset(inst->mem, 0, inst->memcapa);
    inst->running = NU_TRUE;
    inst->time    = 0;
    inst->tps     = 60;

    // Command buffer
    inst->cmds_capa = config->command_buffer_capacity;
    inst->cmds_size = 0;
    inst->cmds      = nux_platform_malloc(inst->userdata,
                                     NUX_MEMORY_USAGE_CORE,
                                     sizeof(*inst->cmds) * inst->cmds_capa);
    NU_CHECK(inst->cmds, goto cleanup0);

    // Initialize resource table
    inst->objects[NU_NULL].type = NUX_OBJECT_NULL;
    inst->objects[NU_NULL].next = NU_NULL;
    for (nu_size_t i = 1; i < NUX_OBJECT_MAX; ++i)
    {
        inst->objects[i].type = NUX_OBJECT_FREE;
        inst->objects[i].next = 0;
    }

    // Initialize inputs
    nu_memset(inst->buttons, 0, sizeof(inst->buttons));
    for (nu_size_t p = 0; p < NUX_PLAYER_MAX; ++p)
    {
        for (nu_size_t a = 0; a < NUX_AXIS_MAX; ++a)
        {
            inst->axis[p][a] = 0;
        }
    }

    // Wasm initialization
    nux_status_t res = nux_wasm_init(inst, config);
    NU_CHECK(res, goto cleanup0);

    return inst;
cleanup0:
    nux_instance_free(inst);
    return NU_NULL;
}
void
nux_instance_free (nux_instance_t inst)
{
    // Wasm
    nux_wasm_free(inst);
    // Command buffer
    if (inst->cmds)
    {
        nux_platform_free(inst->userdata, inst->cmds);
    }
    // State
    if (inst->mem)
    {
        nux_platform_free(inst->userdata, inst->mem);
    }
    // Instance
    nux_platform_free(inst->userdata, inst);
}
void
nux_instance_tick (nux_instance_t inst)
{
    nux_env_t env = nux_instance_init_env(inst);
    nux_wasm_update(env);
    inst->time += delta_time(inst);
}
nux_status_t
nux_instance_load (nux_instance_t inst, const nux_c8_t *cart, nux_u32_t n)
{
    nux_env_t env = nux_instance_init_env(inst);
    return nux_load_cartridge(env, cart, n);
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
nux_env_t
nux_instance_init_env (nux_instance_t inst)
{
    inst->env.inst   = inst;
    inst->env.cursor = NU_V2U_ZEROS;
    inst->env.error  = NUX_ERROR_NONE;
    nu_strncpy(inst->env.error_message, "", sizeof(inst->env.error_message));
    return &inst->env;
}
const nux_c8_t *
nux_instance_get_error (nux_instance_t inst)
{
    return nux_error_message(inst->env.error);
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
        case NUX_ERROR_INVALID_OBJECT_ID:
            return "invalid object id";
        case NUX_ERROR_INVALID_OBJECT_TYPE:
            return "invalid object type";
        case NUX_ERROR_WASM_RUNTIME:
            return "wasm runtime";
        case NUX_ERROR_INVALID_OBJECT_CREATION:
            return "invalid object creation";
        case NUX_ERROR_CART_EOF:
            return "cartridge EOF";
        case NUX_ERROR_CART_MOUNT:
            return "cartridge mount";
        case NUX_ERROR_OUT_OF_POOL_ITEM:
            return "out of pool item";
        case NUX_ERROR_OUT_OF_COMMANDS:
            return "out of commands";
        case NUX_ERROR_OUT_OF_OBJECTS:
            return "out of objects";
    }
    return NU_NULL;
}

void
nux_trace (nux_env_t env, const nux_c8_t *text)
{
    nux_platform_log(env->inst, text, nu_strnlen(text, 1024));
}

void
nux_inspect_i32 (nux_env_t env, const nux_c8_t *name, nux_i32_t *p)
{
    nux_platform_inspect(
        env->inst, name, nu_strnlen(name, NUX_NAME_MAX), NUX_INSPECT_I32, p);
}
void
nux_inspect_f32 (nux_env_t env, const nux_c8_t *name, nux_f32_t *p)
{
    nux_platform_inspect(
        env->inst, name, nu_strnlen(name, NUX_NAME_MAX), NUX_INSPECT_F32, p);
}

nux_u32_t
nux_console_info (nux_env_t env, nux_console_info_t info)
{
    return 0;
}
nux_f32_t
nux_global_time (nux_env_t env)
{
    return env->inst->time;
}
nux_f32_t
nux_delta_time (nux_env_t env)
{
    return delta_time(env->inst);
}
