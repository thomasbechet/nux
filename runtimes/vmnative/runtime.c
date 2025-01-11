#include "runtime.h"

#include "window.h"
#include "renderer.h"

#include <vmcore/platform.h>
#include <vmcore/vm.h>

static nu_byte_t global_heap[NU_MEM_32M];

void
vmn_run (nu_sv_t path)
{
    vmn_error_code_t error;

    vm_config_t config = VM_CONFIG_DEFAULT;
    vmn_error_init();
    error = vmn_window_init();
    NU_CHECK(!error, goto cleanup0);
    error = vmn_renderer_init(&config);
    NU_CHECK(!error, goto cleanup1);

    vm_info_t info;
    nu_memset(&info, 0, sizeof(info));
    info.heap      = global_heap;
    info.heap_size = NU_ARRAY_SIZE(global_heap);
    info.user      = NU_NULL;
    info.specs     = &config;
    vm_t *vm       = vm_init(&info);
    if (!vm)
    {
        VMN_ERROR(VMN_ERROR_VM_INITIALIZATION, NU_NULL);
        goto cleanup2;
    }

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(path, name, NU_PATH_MAX);
    vm_load(vm, name);

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
    vmn_error_print();
    vmn_error_free();
}
