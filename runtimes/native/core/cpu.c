#include "vm.h"

#include "platform.h"

nu_status_t
cpu_init (vm_t *vm, const cpu_config_t *config)
{
    vm->cpu.mem_heap_size  = config->mem_heap_size;
    vm->cpu.mem_stack_size = config->mem_stack_size;
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
