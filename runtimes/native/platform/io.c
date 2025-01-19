#include "logger.h"

static struct
{
    FILE *file;
} io;

void
os_mount (void *user, const nu_char_t *name)
{
    (void)user;
    io.file = fopen((char *)name, "rb");
    NU_ASSERT(io.file);
}
void
os_seek (void *user, nu_size_t n)
{
    (void)user;
    fseek(io.file, n, SEEK_SET);
}
nu_size_t
os_read (void *user, void *p, nu_size_t n)
{
    (void)user;
    return fread(p, n, 1, io.file);
}
void
os_trace (void *user, const nu_char_t *s, nu_size_t n)
{
    logger_log(NU_LOG_INFO, "trace: %*.s", n, s);
}
