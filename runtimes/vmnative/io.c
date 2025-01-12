#include <vmcore/platform.h>

#include "runtime.h"

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
os_log (void *user, nu_log_level_t level, const nu_char_t *fmt, ...)
{
    (void)user;
    nu_char_t buf[256];
    nu_sv_join(buf, sizeof(buf), NU_SV("[VM] "), nu_sv_cstr(fmt));
    va_list args;
    va_start(args, fmt);
    vmn_vlog(level, buf, args);
    va_end(args);
}
