#include "internal.h"

void *
nux_platform_malloc (void *userdata, nux_memory_usage_t usage, nux_u32_t n)
{
    return native_malloc(n);
}
void
nux_platform_free (void *userdata, void *p)
{
    native_free(p);
}
void *
nux_platform_realloc (void *userdata, void *p, nux_u32_t n)
{
    return native_realloc(p, n);
}
void
nux_platform_log (nux_instance_t inst, const nux_c8_t *log, nux_u32_t n)
{
    logger_log(NU_LOG_INFO, "%.*s", n, log);
}
void
nux_platform_debug (nux_instance_t   inst,
                    const nux_c8_t  *name,
                    nux_u32_t        n,
                    nux_debug_type_t type,
                    void            *p)
{
    // Find existing value at address
    intptr_t            addr     = (intptr_t)p;
    runtime_instance_t *instance = nux_instance_get_userdata(inst);
    debug_value_t      *value    = NU_NULL;
    for (nu_size_t i = 0; i < instance->debug_value_count; ++i)
    {
        // if (instance->debug_values[i].addr == addr)
        // {
        //     value = instance->debug_values + i;
        // }
        if (nu_strneq(instance->debug_values[i].name, name, NUX_NAME_MAX))
        {
            value = instance->debug_values + i;
        }
    }
    // Register new value
    if (!value)
    {
        if (instance->debug_value_count
            >= NU_ARRAY_SIZE(instance->debug_values))
        {
            logger_log(NU_LOG_ERROR, "Max inspect value count reach");
            return;
        }
        value       = &instance->debug_values[instance->debug_value_count++];
        value->type = type;
        value->addr = addr;
        value->override = NU_FALSE;
        nu_strncpy(value->name, name, sizeof(value->name));
    }
    // Read / Write value
    if (value->override)
    {
        switch (type)
        {
            case NUX_DEBUG_I32:
                *((nu_i32_t *)p) = value->value.i32;
                break;
            case NUX_DEBUG_F32:
                *((nu_f32_t *)p) = value->value.f32;
                break;
        }
        value->override = NU_FALSE;
    }
    else
    {
        switch (type)
        {
            case NUX_DEBUG_I32:
                value->value.i32 = *(const nu_i32_t *)p;
                break;
            case NUX_DEBUG_F32:
                value->value.f32 = *(const nu_f32_t *)p;
                break;
        }
    }
}
