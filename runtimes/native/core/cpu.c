#include "vm.h"

#include "platform.h"

nu_status_t
cpu_init (vm_t *vm, const cpu_config_t *config)
{
    vm->cpu.config = *config;
    return NU_SUCCESS;
}
void
cpu_free (vm_t *vm)
{
}
nu_status_t
cpu_load_wasm (vm_t *vm, const cart_chunk_header_t *header)
{
    // Load module data
    NU_ASSERT(header->length);
    nu_byte_t *buffer = os_malloc(vm, header->length);
    NU_ASSERT(buffer);
    NU_ASSERT(os_iou_read(vm, buffer, header->length));
    NU_ASSERT(os_cpu_load_wasm(vm, buffer, header->length));
    return NU_SUCCESS;
}
nu_status_t
cpu_call_event (vm_t *vm, cpu_event_t event)
{
    NU_ASSERT(os_cpu_call_event(vm, event));
    return NU_SUCCESS;
}
