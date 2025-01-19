#include <core/platform.h>

static struct
{
    FILE *file;
} io;

void
os_iop_mount (vm_t *vm, const nu_char_t *name)
{
    io.file = fopen((char *)name, "rb");
    NU_ASSERT(io.file);
}
void
os_iop_seek (vm_t *vm, nu_size_t n)
{
    fseek(io.file, n, SEEK_SET);
}
nu_size_t
os_iop_read (vm_t *vm, void *p, nu_size_t n)
{
    return fread(p, n, 1, io.file);
}
