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
    status = cpu_init(vm, &config->cpu);
    NU_CHECK(status, return NU_FAILURE);
    status = gpu_init(vm, &config->gpu);
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
    NU_CHECK(iou_load_cart(vm, name), return NU_FAILURE);
    NU_CHECK(cpu_call_event(vm, CPU_EVENT_START), return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
vm_tick (vm_t *vm, nu_bool_t *exit)
{
    gpu_begin(vm);
    cpu_call_event(vm, CPU_EVENT_UPDATE);
    gpu_end(vm);
    return NU_SUCCESS;
}
