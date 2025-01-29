#include "logger.h"
#include "window.h"
#include "renderer.h"
#include "wasm.h"
#include "core/vm.h"

static nu_byte_t global_heap[NU_MEM_32M];

nu_status_t
runtime_init (const runtime_info_t *info)
{
    nu_status_t status;
    logger_init(info->log_callback);
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);
    status = wasm_init();
    NU_CHECK(status, goto cleanup2);

    vm_t        vm;
    vm_config_t config = VM_CONFIG_DEFAULT;

    status = vm_init(&vm, &config);
    NU_CHECK(status, goto cleanup3);

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(info->path, name, NU_PATH_MAX);
    status = vm_load(&vm, name);
    NU_CHECK(status, goto cleanup4);

    while (vm.running && !window_close_requested())
    {
        window_poll_events();
        vm_tick(&vm);
        window_swap_buffers();
    }

cleanup4:
    vm_free(&vm);
cleanup3:
    wasm_free();
cleanup2:
    renderer_free();
cleanup1:
    window_free();
cleanup0:
    return status;
}
