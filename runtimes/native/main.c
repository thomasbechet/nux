#include "window.h"

#include <nulib.h>
#include <vmcore.h>

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

    nu_vmcore_init(buffer, size);
    nu_window_init();

    while (!nu_window_close_requested())
    {
        nu_window_poll_events();
    }

    nu_vmcore_free();
    nu_window_free();

    return 0;
}
