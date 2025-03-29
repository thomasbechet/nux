#include <nux.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define NU_STDLIB
#include <nulib/nulib.h>

static struct
{
    FILE *file;
} io;

void *
nux_platform_malloc (void *userdata, nux_memory_usage_t usage, nux_u32_t n)
{
    return malloc(n);
}
void
nux_platform_free (void *userdata, void *p)
{
    free(p);
}
void
nux_platform_log (nux_instance_t inst, const nux_c8_t *log, nux_u32_t n)
{
    printf("%.*s\n", (int)n, log);
}
void
nux_platform_inspect (nux_instance_t     inst,
                      const nux_c8_t    *name,
                      nux_u32_t          n,
                      nux_inspect_type_t type,
                      void              *p)
{
}

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

int
main (int argc, char **argv)
{
    nux_instance_config_t config = {};
    nux_instance_t        inst   = nux_instance_init(&config);
    nux_instance_tick(inst);
    nux_instance_free(inst);
    return 0;
}
