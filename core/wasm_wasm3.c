#include "internal.h"

#ifdef NUX_BUILD_WASM3
#include "wasm3_native.c.inc"
#endif

static nux_status_t
nux_wasm3_init (nux_instance_t inst, const nux_instance_config_t *config)
{
    inst->wasm.env     = m3_NewEnvironment();
    inst->wasm.runtime = m3_NewRuntime(inst->wasm.env, 8192, NU_NULL);
    inst->wasm.buffer  = NU_NULL;

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
nux_wasm_load (nux_instance_t inst, nux_u8_t *buffer, nux_u32_t n)
{
    inst->wasm.buffer      = buffer;
    inst->wasm.buffer_size = n;

    M3Result res = m3_ParseModule(inst->wasm.env,
                                  &inst->wasm.module,
                                  inst->wasm.buffer,
                                  inst->wasm.buffer_size);
    if (res)
    {
        return NUX_FAILURE;
    }

    res = m3_LoadModule(inst->wasm.runtime, inst->wasm.module);
    if (res)
    {
        nux_set_error(inst, NUX_ERROR_RUNTIME);
        return NUX_FAILURE;
    }

    link_raw_functions(inst->wasm.module);

    res = m3_FindFunction(
        &inst->wasm.start_function, inst->wasm.runtime, "start");
    if (res)
    {
        nux_set_error(inst, NUX_ERROR_RUNTIME);
        return NUX_FAILURE;
    }
    res = m3_FindFunction(
        &inst->wasm.update_function, inst->wasm.runtime, "update");
    if (res)
    {
        nux_set_error(inst, NUX_ERROR_RUNTIME);
        return NUX_FAILURE;
    }

    return NUX_SUCCESS;
}
nux_status_t
nux_wasm_start (nux_instance_t inst)
{
    M3Result res = m3_Call(inst->wasm.start_function, 0, NU_NULL);
    if (res)
    {
        nux_set_error(inst, NUX_ERROR_RUNTIME);
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_wasm_update (nux_instance_t inst)
{
    if (inst->wasm.buffer)
    {
        M3Result res = m3_Call(inst->wasm.update_function, 0, NU_NULL);
        if (res)
        {
            nux_set_error(inst, NUX_ERROR_RUNTIME);
            return NUX_FAILURE;
        }
    }
    return NUX_SUCCESS;
}
