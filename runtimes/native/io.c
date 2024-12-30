#include "io.h"

#include <platform.h>

static struct
{
    FILE *file;
} _io;

void
os_mount (void *user, const nu_byte_t *name)
{
    printf("os_mount %s\n", name);
    _io.file = fopen((char *)name, "rb");
    NU_ASSERT(_io.file);
}
void
os_seek (void *user, nu_size_t n)
{
    printf("os_seek %lu\n", n);
    fseek(_io.file, n, SEEK_SET);
}
nu_size_t
os_read (void *user, void *p, nu_size_t n)
{
    printf("os_read %lu\n", n);
    return fread(p, n, 1, _io.file);
}
void
os_trace (void *user, const void *str, nu_size_t n)
{
    fprintf(stdout, "%s\n", (char *)str);
}
