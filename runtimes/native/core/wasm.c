#include "vm.h"

#include "platform.h"

nu_status_t
wasm_init (vm_t *vm)
{
    vm->wasm.loaded = NU_FALSE;
    return NU_SUCCESS;
}
void
wasm_free (vm_t *vm)
{
}
nu_status_t
wasm_call_event (vm_t *vm, wasm_event_t event)
{
    if (vm->wasm.loaded)
    {
        return os_cpu_call_event(vm, event);
    }
    return NU_SUCCESS;
}
