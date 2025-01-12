#include "runtime.h"

#include "window.h"
#include "renderer.h"

#include <vmcore/platform.h>
#include <vmcore/vm.h>

static nu_byte_t          global_heap[NU_MEM_32M];
static vmn_log_callback_t log_callback;

nu_status_t
vmn_execute (nu_sv_t path)
{
    nu_status_t status = NU_SUCCESS;

    vm_config_t config = VM_CONFIG_DEFAULT;
    status             = vmn_window_init();
    NU_CHECK(status, goto cleanup0);
    status = vmn_renderer_init(&config);
    NU_CHECK(status, goto cleanup1);

    vm_info_t info;
    nu_memset(&info, 0, sizeof(info));
    info.heap      = global_heap;
    info.heap_size = NU_ARRAY_SIZE(global_heap);
    info.user      = NU_NULL;
    info.specs     = &config;
    vm_t *vm       = vm_init(&info);
    if (!vm)
    {
        status = NU_FAILURE;
        goto cleanup2;
    }

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(path, name, NU_PATH_MAX);
    status = vm_load(vm, name);
    NU_CHECK(status, goto cleanup2);

    while (!vmn_window_close_requested())
    {
        vmn_window_poll_events();
        vmn_renderer_render();
        vmn_window_swap_buffers();
        vm_update(vm);
    }

cleanup2:
    vmn_renderer_free();
cleanup1:
    vmn_window_free();
cleanup0:
    return status;
}
void
vmn_set_log_callback (vmn_log_callback_t callback)
{
    log_callback = callback;
}

void
vmn_vlog (nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    if (log_callback)
    {
        log_callback(level, fmt, args);
    }
}
void
vmn_log (nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vmn_vlog(level, fmt, args);
    va_end(args);
}
