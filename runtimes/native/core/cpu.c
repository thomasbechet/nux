#include "vm.h"

#include "platform.h"

nu_status_t
cpu_init (vm_t *vm, const cpu_config_t *config)
{
    vm->cpu.ram      = os_malloc(vm, config->ram_capacity);
    vm->cpu.ram_capa = config->ram_capacity;
    vm->cpu.ram_size = 0;
    return NU_SUCCESS;
}
void
cpu_free (vm_t *vm)
{
}
nu_status_t
cpu_call_event (vm_t *vm, cpu_event_t event)
{
    NU_ASSERT(os_cpu_call_event(vm, event));
    return NU_SUCCESS;
}
void *
cpu_malloc (vm_t *vm, nu_u32_t n)
{
    if (vm->cpu.ram_size + n > vm->cpu.ram_capa)
    {
        vm_log(vm, NU_LOG_ERROR, "out of cpu memory");
        return NU_NULL;
    }
    void *p = vm->cpu.ram + vm->cpu.ram_size;
    vm->cpu.ram_size += n;
    return p;
}
