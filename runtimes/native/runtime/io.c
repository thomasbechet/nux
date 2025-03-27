#include "runtime.h"

static struct
{
    FILE *file;
} io;

void
nux_platform_mount (nux_instance_t inst, const nux_c8_t *name, nux_u32_t n)
{
    io.file = fopen((char *)name, "rb");
    NU_ASSERT(io.file);
}
void
nux_platform_seek (nux_instance_t inst, nux_u32_t n)
{
    fseek(io.file, n, SEEK_SET);
}
nux_u32_t
nux_platform_read (nux_instance_t inst, void *p, nux_u32_t n)
{
    NU_CHECK(fread(p, n, 1, io.file) == 1, return 0);
    return n;
}
