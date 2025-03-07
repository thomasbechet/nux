#include "runtime.h"
#include "core/platform.h"

void
os_inspect (vm_t *vm, const nu_char_t *name, sys_inspect_type_t type, void *p)
{
    // Find existing value at address
    nu_u32_t         addr     = (nu_u32_t)((nu_size_t)p - (nu_size_t)vm->mem);
    runtime_instance_t      *instance = vm->userdata;
    inspect_value_t *value    = NU_NULL;
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
        nu_sv_to_cstr(nu_sv_cstr(name), value->name, sizeof(value->name));
    }
    // Read / Write value
    if (value->override)
    {
        switch (type)
        {
            case SYS_INSPECT_I32:
                *((nu_i32_t *)p) = value->value.i32;
                break;
            case SYS_INSPECT_F32:
                *((nu_f32_t *)p) = value->value.f32;
                break;
        }
        value->override = NU_FALSE;
    }
    else
    {
        switch (type)
        {
            case SYS_INSPECT_I32:
                value->value.i32 = *(const nu_i32_t *)p;
                break;
            case SYS_INSPECT_F32:
                value->value.f32 = *(const nu_f32_t *)p;
                break;
        }
    }
}
