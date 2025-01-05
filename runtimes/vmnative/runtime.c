#include "runtime.h"

#include "window.h"
#include "renderer.h"

#include <vmcore/platform.h>
#include <vmcore/vm.h>

static nu_byte_t *
load_bytes (const char *filename, nu_size_t *size)
{
    FILE *f = fopen((char *)filename, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", filename);
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)malloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}

static nu_byte_t global_heap[NU_MEM_1M];

void
nux_runtime_run (int argc, const char **argv)
{
    nux_error_code_t error;

    nux_vm_config_t config = NUX_CONFIG_DEFAULT;
    nux_error_init();
    error = nux_window_init();
    NU_CHECK(!error, goto cleanup0);
    error = nux_renderer_init(&config);
    NU_CHECK(!error, goto cleanup1);

    nux_vm_info_t info;
    nu_memset(&info, 0, sizeof(info));
    info.heap      = global_heap;
    info.heap_size = NU_ARRAY_SIZE(global_heap);
    info.user      = NU_NULL;
    info.specs     = &config;
    nux_vm_t *vm   = nux_vm_init(&info);
    if (!vm)
    {
        NUX_ERROR(NUX_ERROR_VM_INITIALIZATION, NU_NULL);
        goto cleanup2;
    }

    const nu_byte_t *name = (const nu_byte_t *)argv[0];
    nux_vm_load(vm, name);

    while (!nux_window_close_requested())
    {
        nux_window_poll_events();
        nux_renderer_render();
        nux_window_swap_buffers();
        nux_vm_update(vm);
    }

cleanup2:
    nux_renderer_free();
cleanup1:
    nux_window_free();
cleanup0:
    nux_error_print();
    nux_error_free();
}
