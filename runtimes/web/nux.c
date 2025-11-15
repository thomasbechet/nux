#include <nux.h>
#include <string.h>
#include <stdlib.h>

void *
nux_os_alloc (void *p, nux_u32_t osize, nux_u32_t nsize)
{
    if (!p)
    {
        return malloc(nsize);
    }
    else if (!nsize)
    {
        free(p);
        return NULL;
    }
    else
    {
        return realloc(p, nsize);
    }
}

void
nux_os_hotreload_add (const nux_c8_t *path, nux_rid_t handle)
{
}
void
nux_os_hotreload_remove (nux_rid_t handle)
{
}
void
nux_os_hotreload_pull (nux_rid_t *handles, nux_u32_t *count)
{
    *count = 0;
}
void
nux_os_stats_update (nux_u64_t *stats)
{
    stats[NUX_STAT_FPS]           = 100;
    stats[NUX_STAT_SCREEN_WIDTH]  = 1600;
    stats[NUX_STAT_SCREEN_HEIGHT] = 900;
    stats[NUX_STAT_TIMESTAMP]     = 1;
}

void
instance_init (void)
{
    nux_core_init();
}
void
instance_update (void)
{
    nux_core_update();
}
void
instance_free (void)
{
    nux_core_free();
}
