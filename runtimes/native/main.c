#include "window.h"

#include <nulib.h>
#include <vm.h>

void os_draw(void);

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

int
main (int argc, char **argv)
{
    nu_size_t  size;
    nu_byte_t *buffer = load_bytes(argv[1], &size);

    nux_vm_info_t info;
    nu_memset(&info, 0, sizeof(info));
    nux_vm_t vm = nux_vm_init(&info);
    NU_ASSERT(vm);
    nu_window_init();

    while (!nu_window_close_requested())
    {
        nu_window_poll_events();
    }

    nu_window_free();

    return 0;
}
