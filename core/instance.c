#include "internal.h"

struct nux_env
nux_env_init (nux_instance_t inst, nux_oid_t scene)
{
    struct nux_env env;
    env.inst         = inst;
    env.active_scope = NUX_NULL;
    env.cursor       = NU_V2U_ZEROS;
    nux_bind_scene(&env, scene);
    return env;
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

    // Initialize resource table
    inst->objects[NUX_NULL].type = NUX_OBJECT_NULL;
    inst->objects[NUX_NULL].next = NUX_NULL;
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
    nu_status_t res = nux_wasm_init(inst, config);
    NU_CHECK(res, goto cleanup0);

    return inst;
cleanup0:
    nux_instance_free(inst);
    return NU_NULL;
}
void
nux_instance_free (nux_instance_t inst)
{
    nux_wasm_free(inst);
    if (inst->mem)
    {
        nux_platform_free(inst->userdata, inst->mem);
    }
    nux_platform_free(inst->userdata, inst);
}
void
nux_instance_tick (nux_instance_t inst)
{
    nux_wasm_update(inst);
}
void
nux_instance_load (nux_instance_t inst, const nux_u8_t *data, nux_u32_t n)
{
    nux_wasm_load(inst, data, n);
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
nux_instance_set_userdata (nux_instance_t inst, void *userdata)
{
    inst->userdata = userdata;
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
        env->inst, name, nu_strnlen(name, NUX_NAME_MAX), NUX_INSPECT_U32, p);
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
    return 1.0 / (nu_f32_t)env->inst->tps;
}
