#include "vm.h"

#include "cpu.h"
#include "gpu.h"
#include "platform.h"

nu_status_t
vm_init (vm_t *vm, const vm_config_t *config)
{
    nu_memset(vm, 0, sizeof(*vm));

    // Init units
    nu_status_t status;
    status = boot_init(vm);
    NU_CHECK(status, return NU_FAILURE);
    status = cpu_init(vm, &config->cpu);
    NU_CHECK(status, return NU_FAILURE);
    status = gpu_init(vm, &config->gpu);
    NU_CHECK(status, return NU_FAILURE);
    status = gpad_init(vm);
    NU_CHECK(status, return NU_FAILURE);

    vm->running = NU_TRUE;

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
    NU_CHECK(boot_load_cart(vm, name), return NU_FAILURE);
    NU_CHECK(cpu_call_event(vm, CPU_EVENT_START), return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
vm_tick (vm_t *vm)
{
    gpad_update(vm);
    gpu_begin(vm);
    cpu_call_event(vm, CPU_EVENT_UPDATE);
    gpu_end(vm);
    return NU_SUCCESS;
}
void
vm_save_state (const vm_t *vm, void *buf)
{
    nu_byte_t *p = buf;
    nu_memcpy(p, vm->gpu.vram, vm->gpu.vram_capa);
    p += vm->gpu.vram_capa;
}
nu_status_t
vm_load_state (vm_t *vm, const void *buf)
{
    const nu_byte_t *p = buf;
    nu_memcpy(vm->gpu.vram, p, vm->gpu.vram_capa);
    // TODO: update backend
    return NU_SUCCESS;
}

void
vm_config_default (vm_config_t *config)
{
    config->cpu.mem_heap_size  = NU_MEM_1M;
    config->cpu.mem_stack_size = NU_MEM_64K;
    config->gpu.vram_capacity  = NU_MEM_16M;
}
nu_size_t
vm_config_state_memsize (const vm_config_t *config)
{
    return config->gpu.vram_capacity;
}

void
vm_log (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vm_vlog(vm, level, fmt, args);
    va_end(args);
}
void
vm_vlog (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    os_vlog(vm, level, fmt, args);
}
