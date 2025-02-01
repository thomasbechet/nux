#include "core/platform.h"

static struct
{
    FILE *file;
} io;

nu_status_t
os_cart_mount (vm_t *vm, const nu_char_t *name)
{
    io.file = fopen((char *)name, "rb");
    NU_ASSERT(io.file);
    return NU_SUCCESS;
}
nu_status_t
os_cart_seek (vm_t *vm, nu_size_t n)
{
    fseek(io.file, n, SEEK_SET);
    return NU_SUCCESS;
}
nu_size_t
os_cart_read (vm_t *vm, void *p, nu_size_t n)
{
    NU_CHECK(fread(p, n, 1, io.file) == 1, return 0);
    return n;
}
