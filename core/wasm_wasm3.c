#include "internal.h"

#ifdef NUX_BUILD_WASM3
#include "wasm3_native.c.inc"
#endif

static nux_status_t
nux_wasm3_init (nux_instance_t inst, const nux_instance_config_t *config)
{
    inst->wasm.env     = m3_NewEnvironment();
    inst->wasm.runtime = m3_NewRuntime(inst->wasm.env, 8192, &inst->env);
    inst->wasm.buffer  = NU_NULL;
    inst->wasm.started = NU_FALSE;
    return NUX_SUCCESS;
}

nux_status_t
nux_wasm_init (nux_instance_t inst, const nux_instance_config_t *config)
{
    return nux_wasm3_init(inst, config);
}
void
nux_wasm_free (nux_instance_t inst)
{
    if (inst->wasm.buffer)
    {
        nux_platform_free(inst->userdata, inst->wasm.buffer);
    }
    m3_FreeRuntime(inst->wasm.runtime);
    m3_FreeEnvironment(inst->wasm.env);
}
nux_status_t
nux_wasm_load (nux_env_t env, nux_u8_t *buffer, nux_u32_t n)
{
    env->inst->wasm.buffer      = buffer;
    env->inst->wasm.buffer_size = n;

    M3Result res = m3_ParseModule(env->inst->wasm.env,
                                  &env->inst->wasm.module,
                                  env->inst->wasm.buffer,
                                  env->inst->wasm.buffer_size);
    if (res)
    {
        return NUX_FAILURE;
    }

    res = m3_LoadModule(env->inst->wasm.runtime, env->inst->wasm.module);
    if (res)
    {
        nux_set_error(env, NUX_ERROR_WASM_RUNTIME);
        return NUX_FAILURE;
    }

    link_raw_functions(env->inst->wasm.module);

    res = m3_FindFunction(
        &env->inst->wasm.start_function, env->inst->wasm.runtime, "start");
    if (res)
    {
        nux_set_error(env, NUX_ERROR_WASM_RUNTIME);
        return NUX_FAILURE;
    }
    res = m3_FindFunction(
        &env->inst->wasm.update_function, env->inst->wasm.runtime, "update");
    if (res)
    {
        nux_set_error(env, NUX_ERROR_WASM_RUNTIME);
        return NUX_FAILURE;
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_wasm_start (nux_env_t env)
{
    M3Result res = m3_Call(env->inst->wasm.start_function, 0, NU_NULL);
    if (res)
    {
        nux_set_error(env, NUX_ERROR_WASM_RUNTIME);
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_wasm_update (nux_env_t env)
{
    if (env->inst->wasm.buffer)
    {
        if (!env->inst->wasm.started)
        {
            nux_wasm_start(env);
            env->inst->wasm.started = NU_TRUE;
        }
        M3Result res = m3_Call(env->inst->wasm.update_function, 0, NU_NULL);
        if (res)
        {
            nux_set_error(env, NUX_ERROR_WASM_RUNTIME);
            return NUX_FAILURE;
        }
    }
    return NUX_SUCCESS;
}
