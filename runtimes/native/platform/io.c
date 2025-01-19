#include "io.h"

#include "../iop.h"

static struct
{
    FILE *file;
} _io;

void
os_mount (void *user, const nu_char_t *name)
{
    (void)user;
    _io.file = fopen((char *)name, "rb");
    NU_ASSERT(_io.file);
}
void
os_seek (void *user, nu_size_t n)
{
    (void)user;
    fseek(_io.file, n, SEEK_SET);
}
nu_size_t
os_read (void *user, void *p, nu_size_t n)
{
    (void)user;
    return fread(p, n, 1, _io.file);
}
void
os_trace (void *user, const nu_char_t *s, nu_size_t n)
{
    iop_log(user, NU_LOG_INFO, "trace: %*.s", n, s);
}
