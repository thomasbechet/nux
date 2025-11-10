#include "internal.h"

static nux_status_t
init_module (nux_module_t *module)
{
    if (module->initialized)
    {
        return NUX_SUCCESS;
    }
    NUX_INFO("intitializing module '%s'", module->info.name);
    if (module->info.data && module->info.size)
    {
        nux_memset(module->info.data, 0, module->info.size);
    }
    if (module->info.init)
    {
        NUX_ENSURE(module->info.init(),
                   return NUX_FAILURE,
                   "failed to init '%s' module",
                   module->info.name);
    }
    module->initialized = NUX_TRUE;
    return NUX_SUCCESS;
}
static void
free_module (nux_module_t *module)
{
    if (module->initialized)
    {
        NUX_INFO("terminating module '%s'", module->info.name);
        if (module->info.free)
        {
            module->info.free();
        }
        module->initialized = NUX_FALSE;
    }
}

void
nux_register_module (nux_module_info_t info)
{
    nux_core_module_t *core = nux_core();
    nux_module_t      *m    = nux_module_vec_push(&core->modules);
    nux_memset(m, 0, sizeof(*m));
    m->info        = info;
    m->initialized = NUX_FALSE;
}
nux_status_t
nux_module_requires (const nux_c8_t *name)
{
    nux_core_module_t *core = nux_core();
    for (nux_u32_t i = 0; i < core->modules.size; ++i)
    {
        nux_module_t *module = core->modules.data + i;
        if (nux_strncmp(module->info.name, name, NUX_NAME_MAX) == 0)
        {
            return init_module(module);
        }
    }

    NUX_ERROR("module '%s' not found", name);
    return NUX_FAILURE;
}

void
nux_module_free_all (void)
{
    nux_core_module_t *core = nux_core();
    for (nux_u32_t i = 0; i < core->modules.size; ++i)
    {
        free_module(core->modules.data + i);
    }
}
