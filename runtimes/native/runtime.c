#include "runtime.h"

#include "platform.h"
#include "window.h"

#include <vm.h>

static FILE *vmfile;

void
os_mount (void *user, const nu_byte_t *name)
{
    vmfile = fopen((char *)name, "rb");
    NU_ASSERT(vmfile);
}
void
os_seek (void *user, nu_size_t n)
{
    fseek(vmfile, n, SEEK_SET);
}
nu_size_t
os_read (void *user, void *p, nu_size_t n)
{
    return fread(p, n, 1, vmfile);
}

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
    nu_window_init();

    nux_vm_info_t info;
    nu_memset(&info, 0, sizeof(info));
    info.heap      = global_heap;
    info.heap_size = NU_ARRAY_SIZE(global_heap);
    nux_vm_t *vm   = nux_vm_init(&info);
    NU_ASSERT(vm);

    const nu_byte_t *name = (const nu_byte_t *)argv[0];
    nux_vm_load(vm, name);

    while (!nu_window_close_requested())
    {
        nu_window_poll_events();
    }

    nu_window_free();
}
