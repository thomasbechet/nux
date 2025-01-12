#include "runtime.h"

#include "window.h"
#include "renderer.h"

#include <vmcore/platform.h>
#include <vmcore/vm.h>

static nu_byte_t global_heap[NU_MEM_32M];

void
vmn_execute (nu_sv_t path)
{
    nu_status_t status = NU_SUCCESS;
    vmn_error_t error;

    vm_config_t config = VM_CONFIG_DEFAULT;
    status             = vmn_window_init(&error);
    NU_CHECK(status, goto cleanup0);
    status = vmn_renderer_init(&config, &error);
    NU_CHECK(status, goto cleanup1);

    vm_info_t info;
    nu_memset(&info, 0, sizeof(info));
    info.heap      = global_heap;
    info.heap_size = NU_ARRAY_SIZE(global_heap);
    info.user      = NU_NULL;
    info.specs     = &config;
    vm_t *vm       = vm_init(&info, &error.vm);
    if (!vm)
    {
        error.code = VMN_ERROR_VM;
        status     = NU_FAILURE;
        goto cleanup2;
    }

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(path, name, NU_PATH_MAX);
    status = vm_load(vm, name, &error.vm);
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
    if (!status)
    {
        vmn_error_print(&error);
    }
}
