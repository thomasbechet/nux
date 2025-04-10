#include "internal.h"

void
nux_platform_inspect (nux_instance_t     inst,
                      const nux_c8_t    *name,
                      nux_u32_t          n,
                      nux_inspect_type_t type,
                      void              *p)
{
    // Find existing value at address
    intptr_t            addr     = (intptr_t)p;
    runtime_instance_t *instance = nux_instance_get_userdata(inst);
    inspect_value_t    *value    = NU_NULL;
    for (nu_size_t i = 0; i < instance->inspect_value_count; ++i)
    {
        if (instance->inspect_values[i].addr == addr)
        {
            value = instance->inspect_values + i;
        }
    }
    // Register new value
    if (!value)
    {
        if (instance->inspect_value_count
            >= NU_ARRAY_SIZE(instance->inspect_values))
        {
            logger_log(NU_LOG_ERROR, "Max inspect value count reach");
            return;
        }
        value = &instance->inspect_values[instance->inspect_value_count++];
        value->type     = type;
        value->addr     = addr;
        value->override = NU_FALSE;
        nu_strncpy(value->name, name, sizeof(value->name));
    }
    // Read / Write value
    if (value->override)
    {
        switch (type)
        {
            case NUX_INSPECT_I32:
                *((nu_i32_t *)p) = value->value.i32;
                break;
            case NUX_INSPECT_F32:
                *((nu_f32_t *)p) = value->value.f32;
                break;
        }
        value->override = NU_FALSE;
    }
    else
    {
        switch (type)
        {
            case NUX_INSPECT_I32:
                value->value.i32 = *(const nu_i32_t *)p;
                break;
            case NUX_INSPECT_F32:
                value->value.f32 = *(const nu_f32_t *)p;
                break;
        }
    }
}
