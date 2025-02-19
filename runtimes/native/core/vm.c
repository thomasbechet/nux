#include "vm.h"

#include "platform.h"
#include "bios.h"

#include <string.h>

nu_u32_t
vm_malloc (vm_t *vm, nu_u32_t n)
{
    if (vm->memsize + n > vm->memcapa)
    {
        vm_log(vm, NU_LOG_ERROR, "out of memory");
        return ADDR_INVALID;
    }
    nu_u32_t p = vm->memsize;
    vm->memsize += n;
    return p;
}
resource_t *
vm_set_res (vm_t *vm, nu_u32_t id, resource_type_t type)
{
    if (type == RES_NULL)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid resource type");
        return NU_NULL;
    }
    if (id == 0 || id > MAX_RESOURCE_COUNT)
    {
        vm_log(vm, NU_LOG_ERROR, "Invalid resource id");
        return NU_NULL;
    }
    // TODO: uninit resource
    switch (vm->res[id].type)
    {
        case RES_POOL:
        case RES_TEXTURE:
        case RES_MESH:
        case RES_MODEL:
        case RES_SPRITESHEET:
            break;
        default:
            break;
    }
    vm->res[id].type = type;
    vm->res[id].next = 0;
    return vm->res + id;
}
resource_t *
vm_get_res (vm_t *vm, nu_u32_t id, resource_type_t type)
{
    if (!id || id > MAX_RESOURCE_COUNT)
    {
        return NU_NULL;
    }
    resource_t *res = vm->res + id;
    return res->type == type ? res : NU_NULL;
}

nu_u32_t
sys_add_group (vm_t *vm, nu_u32_t size)
{
    return 0;
}
void
sys_clear_group (vm_t *vm, nu_u32_t group)
{
}

nu_status_t
vm_init (vm_t *vm, const vm_config_t *config)
{
    nu_memset(vm, 0, sizeof(*vm));

    // Init units
    nu_status_t status;
    status = wasm_init(vm);
    NU_CHECK(status, return NU_FAILURE);
    status = gfx_init(vm);
    NU_CHECK(status, return NU_FAILURE);
    status = gpad_init(vm);
    NU_CHECK(status, return NU_FAILURE);

    vm->running = NU_TRUE;
    vm->time    = 0;

    // Allocate memory
    vm->mem = os_malloc(vm, config->memsize);
    NU_CHECK(vm->mem, return NU_FAILURE);
    vm->memsize = 0;
    vm->memcapa = config->memsize;

    // Initialize resource table
    vm->res[ID_NULL].type = RES_NULL;
    vm->res[ID_NULL].next = 0;
    for (nu_size_t i = 1; i < MAX_RESOURCE_COUNT; ++i)
    {
        vm->res[i].type = RES_FREE;
        vm->res[i].next = 0;
    }

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
    NU_CHECK(wasm_call_event(vm, WASM_EVENT_START), return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
vm_tick (vm_t *vm)
{
    gpad_update(vm);
    gfx_begin_frame(vm);
    wasm_call_event(vm, WASM_EVENT_UPDATE);
    gfx_end_frame(vm);
    vm->time += 1.0 / vm->tps;
    return NU_SUCCESS;
}
void
vm_save_state (const vm_t *vm, void *state)
{
    // TODO: optimize nu_memcpy
    nu_byte_t *p = state;
    memcpy(p, vm->mem, vm->memcapa);
    p += vm->memsize;
}
nu_status_t
vm_load_state (vm_t *vm, const void *state)
{
    const nu_byte_t *p = state;
    memcpy(vm->mem, p, vm->memcapa);
    p += vm->memcapa;
    // TODO: update backend
    // gfx_reload_state(vm);
    return NU_SUCCESS;
}

void
vm_config_default (vm_config_t *config)
{
    config->memsize = NU_MEM_128M;
    config->tps     = 30;
}
nu_size_t
vm_config_state_memsize (const vm_config_t *config)
{
    return sizeof(vm_t) + config->memsize;
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
nu_u32_t
sys_console_info (vm_t *vm, sys_console_info_t info)
{
    switch (info)
    {
        case SYS_CONSOLE_MEMORY_CAPACITY:
            return vm->memcapa;
        case SYS_CONSOLE_MEMORY_USAGE:
            return vm->memsize;
    }
    return 0;
}
nu_f32_t
sys_time (vm_t *vm)
{
    return vm->time;
}
