#include "vm.h"

#include "iou.h"
#include "cpu.h"
#include "gpu.h"

nu_status_t
vm_init (vm_t *vm, const vm_config_t *config)
{
    nu_memset(vm, 0, sizeof(*vm));

    // Init units
    nu_status_t status;
    status = iou_init(vm);
    NU_CHECK(status, return NU_FAILURE);
    status = cpu_init(vm, config);
    NU_CHECK(status, return NU_FAILURE);
    status = gpu_init(vm, config);
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
    return iou_load_full(vm, name);
}
nu_status_t
vm_tick (vm_t *vm, nu_bool_t *exit)
{
    cpu_update(vm);
    gpu_render(vm);
    return NU_SUCCESS;
}
