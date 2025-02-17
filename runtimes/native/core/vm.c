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
nu_u32_t
vm_add_res (vm_t *vm, resource_type_t type)
{
    if (!vm->res_free)
    {
        vm_log(vm, NU_LOG_ERROR, "out of resource");
        return ID_NULL;
    }
    nu_u32_t index      = vm->res_free;
    vm->res_free        = vm->res[index].next;
    vm->res[index].type = type;
    vm->res[index].hash = 0;
    return INDEX_TO_ID(index);
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
nu_u32_t
sys_find (vm_t *vm, const nu_char_t *name)
{
    nu_u32_t id = sys_find_hash(vm, nu_sv_hash(nu_sv_cstr(name)));
    if (!id)
    {
        vm_log(vm, NU_LOG_ERROR, "Resource not found %s", name);
    }
    return id;
}
nu_u32_t
sys_find_hash (vm_t *vm, nu_u32_t hash)
{
    for (nu_size_t i = 0; i < MAX_RESOURCE_COUNT; ++i)
    {
        if (hash == vm->res[i].hash)
        {
            return INDEX_TO_ID(i);
        }
    }
    vm_log(vm, NU_LOG_ERROR, "resource not found 0x%x", hash);
    return ID_NULL;
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

    // Allocate memory
    vm->mem = os_malloc(vm, config->memsize);
    NU_CHECK(vm->mem, return NU_FAILURE);
    vm->memsize = 0;
    vm->memcapa = config->memsize;

    // Initialize resource table
    for (nu_size_t i = 0; i < MAX_RESOURCE_COUNT; ++i)
    {
        vm->res[i].type = RES_FREE;
        vm->res[i].next = INDEX_TO_ID(i + 1);
    }
    vm->res[MAX_RESOURCE_COUNT - 1].next = ID_NULL;
    vm->res_free                         = INDEX_TO_ID(0);

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
