#include "internal.h"

#ifdef NUX_BUILD_WASM3
#include "wasm3_native.c.inc"
#endif

static nu_status_t
nux_wasm3_init (nux_instance_t inst, const nux_instance_config_t *config)
{
    inst->wasm.env     = m3_NewEnvironment();
    inst->wasm.runtime = m3_NewRuntime(inst->wasm.env, 8192, NU_NULL);

    return NU_SUCCESS;
}

nu_status_t
nux_wasm_init (nux_instance_t inst, const nux_instance_config_t *config)
{
    return nux_wasm3_init(inst, config);
}
void
nux_wasm_free (nux_instance_t inst)
{
    m3_FreeRuntime(inst->wasm.runtime);
    m3_FreeEnvironment(inst->wasm.env);
}
nu_status_t
nux_wasm_load (nux_instance_t inst, const nux_u8_t *data, nux_u32_t n)
{
    M3Result res = m3_ParseModule(
        inst->wasm.env, &inst->wasm.module, (const uint8_t *)data, n);
    if (res)
    {
        return NU_FAILURE;
    }

    res = m3_LoadModule(inst->wasm.runtime, inst->wasm.module);
    if (res)
    {
        return NU_FAILURE;
    }

    link_raw_functions(inst->wasm.module);

    res = m3_FindFunction(
        &inst->wasm.start_function, inst->wasm.runtime, "start");
    if (res)
    {
        return NU_FAILURE;
    }
    res = m3_FindFunction(
        &inst->wasm.update_function, inst->wasm.runtime, "update");
    if (res)
    {
        return NU_FAILURE;
    }

    inst->wasm.loaded = NU_TRUE;

    return NU_SUCCESS;
}
nu_status_t
nux_wasm_start (nux_instance_t inst)
{
    M3Result res = m3_Call(inst->wasm.start_function, 0, NU_NULL);
    if (res)
    {
    }
    return NU_SUCCESS;
}
nu_status_t
nux_wasm_update (nux_instance_t inst)
{
    M3Result res = m3_Call(inst->wasm.update_function, 0, NU_NULL);
    if (res)
    {
    }
    return NU_SUCCESS;
}
