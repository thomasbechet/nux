#include "internal.h"

static struct
{
    FILE *file;
} io;

nux_status_t
nux_platform_mount (nux_instance_t inst, const nux_c8_t *name, nux_u32_t n)
{
    io.file = fopen((char *)name, "rb");
    return io.file ? NUX_SUCCESS : NUX_FAILURE;
}
nux_status_t
nux_platform_seek (nux_instance_t inst, nux_u32_t n)
{
    fseek(io.file, n, SEEK_SET);
    return NUX_SUCCESS;
}
nux_u32_t
nux_platform_read (nux_instance_t inst, void *p, nux_u32_t n)
{
    NU_CHECK(fread(p, n, 1, io.file) == 1, return 0);
    return n;
}
