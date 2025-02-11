#include "vm.h"

#include "cpu.h"
#include "gpu.h"
#include "platform.h"
#include "bios.h"

nu_status_t
vm_init (vm_t *vm, const vm_config_t *config)
{
    nu_memset(vm, 0, sizeof(*vm));

    // Init units
    nu_status_t status;
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
    NU_CHECK(bios_load_cart(vm, name), return NU_FAILURE);
    NU_CHECK(cpu_call_event(vm, CPU_EVENT_START), return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
vm_tick (vm_t *vm)
{
    gpad_update(vm);
    gpu_begin_frame(vm);
    cpu_call_event(vm, CPU_EVENT_UPDATE);
    gpu_end_frame(vm);
    return NU_SUCCESS;
}
void
vm_save_state (const vm_t *vm, void *state)
{
    nu_byte_t *p = state;
    nu_memcpy(p, vm->gpu.vram, vm->gpu.vram_capa);
    p += vm->gpu.vram_capa;
}
nu_status_t
vm_load_state (vm_t *vm, const void *state)
{
    const nu_byte_t *p = state;
    nu_memcpy(vm->gpu.vram, p, vm->gpu.vram_capa);
    // TODO: update backend
    return NU_SUCCESS;
}

void
vm_config_default (vm_config_t *config)
{
    config->cpu.ram_capacity  = NU_MEM_256M;
    config->gpu.vram_capacity = NU_MEM_32M;
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

void
sys_trace (vm_t *vm, const nu_char_t *text)
{
    vm_log(vm, NU_LOG_INFO, text);
}
