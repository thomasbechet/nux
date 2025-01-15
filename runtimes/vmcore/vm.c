#include "vm.h"

#include "wasm.h"
#include "io.h"
#include "platform.h"

#include <nulib.h>

vm_t *
vm_init (const vm_info_t *info)
{
    NU_ASSERT(info->heap_size > sizeof(vm_t));
    vm_t *vm = info->heap;
    nu_memset(vm, 0, sizeof(*vm));
    vm->heap_ptr  = (void *)((nu_size_t)info->heap + sizeof(*vm));
    vm->heap      = info->heap;
    vm->heap_size = info->heap_size;
    vm->config    = *info->specs;
    vm->user      = info->user;

    nu_status_t status;
    status = vm_io_init(vm);
    NU_CHECK(status, return NU_NULL);
    status = vm_wasm_init(vm);
    NU_CHECK(status, return NU_NULL);

    return vm;
}
nu_status_t
vm_load (vm_t *vm, const nu_char_t *name)
{
    return vm_cart_load_full(vm, name);
}
void
vm_update (vm_t *vm)
{
    vm_wasm_update(vm);
}

void *
vm_malloc (vm_t *vm, nu_size_t n)
{
    if ((nu_size_t)vm->heap_ptr > (nu_size_t)vm->heap + vm->heap_size)
    {
        vm_log(vm, NU_LOG_ERROR, "Out of memory");
        return NU_NULL;
    }
    void *ptr    = vm->heap_ptr;
    vm->heap_ptr = (void *)((nu_size_t)vm->heap_ptr + n);
    return ptr;
}
void
vm_log (vm_t *vm, nu_log_level_t level, const nu_char_t *format, ...)
{
    va_list args;
    va_start(args, format);
    os_vlog(vm->user, level, format, args);
    va_end(args);
}
