#include <nux.h>
#include <string.h>
#include <stdlib.h>

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
start (void)
{
    nux_config_t config;
    memset(&config, 0, sizeof(config));
    config.max_id_count  = 4096;
    config.memory_size    = (1 << 25);
    config.width          = 800;
    config.height         = 400;
    config.tick_frequency = 60;
    config.userdata       = NULL;

    nux_ctx_t *ctx = nux_instance_init(&config);
    for (nux_u32_t i = 0; i < 10; ++i)
    {
        nux_instance_tick(ctx);
    }
    nux_instance_free(ctx);
}
