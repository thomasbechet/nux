#include <nux.h>
#include <string.h>
#include <stdlib.h>

static nux_ctx_t *ctx;

void *
nux_os_alloc (void *userdata, void *p, nux_u32_t osize, nux_u32_t nsize)
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
nux_os_hotreload_add (void *userdata, const nux_c8_t *path, nux_rid_t handle)
{
}
void
nux_os_hotreload_remove (void *userdata, nux_rid_t handle)
{
}
void
nux_os_hotreload_pull (void *userdata, nux_rid_t *handles, nux_u32_t *count)
{
    *count = 0;
}
void
nux_os_stats_update (void *userdata, nux_u64_t *stats)
{
    stats[NUX_STAT_FPS]           = 100;
    stats[NUX_STAT_SCREEN_WIDTH]  = 1600;
    stats[NUX_STAT_SCREEN_HEIGHT] = 900;
    stats[NUX_STAT_TIMESTAMP]     = 1;
}

void
instance_init (void)
{
    ctx = nux_instance_init(NULL, "cart.bin");
}
void
instance_update (void)
{
    nux_instance_update(ctx);
}
void
instance_free (void)
{
    nux_instance_free(ctx);
}
