#include "vm.h"

#include "iop.h"
#include "cpu.h"
#include "gpu.h"
#include "platform.h"

nu_status_t
vm_init (vm_t *vm, const vm_config_t *config)
{
    nu_memset(vm, 0, sizeof(*vm));
    vm->config = *config;

    // Init units
    nu_status_t status;
    status = iop_init(vm);
    NU_CHECK(status, return NU_FAILURE);
    status = cpu_init(vm);
    NU_CHECK(status, return NU_FAILURE);
    status = gpu_init(vm);
    NU_CHECK(status, return NU_FAILURE);

    return NU_SUCCESS;
}
void
vm_free (vm_t *vm)
{
    // Free units
}
nu_status_t
vm_load (vm_t *vm, const nu_char_t *name)
{
    return iop_load_full(vm, name);
}
nu_status_t
vm_tick (vm_t *vm, nu_bool_t *exit)
{
    cpu_update(vm);
    gpu_render(vm);
    return NU_SUCCESS;
}
