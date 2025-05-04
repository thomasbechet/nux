#include "internal.h"

void *
nux_platform_malloc (void *userdata, nux_memory_usage_t usage, nux_u32_t n)
{
    return native_malloc(n);
}
void
nux_platform_free (void *userdata, void *p)
{
    native_free(p);
}
void *
nux_platform_realloc (void *userdata, void *p, nux_u32_t n)
{
    return native_realloc(p, n);
}
void
nux_platform_log (nux_instance_t inst, const nux_c8_t *log, nux_u32_t n)
{
    logger_log(NU_LOG_INFO, "%.*s", n, log);
}
void
nux_platform_debug (nux_instance_t   inst,
                    const nux_c8_t  *name,
                    nux_u32_t        n,
                    nux_debug_type_t type,
                    void            *p)
{
}
