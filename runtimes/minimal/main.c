#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <nux.h>

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
nux_status_t
nux_os_open (void *userdata, nux_u32_t slot, const nux_c8_t *url, nux_u32_t n)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_seek (void *userdata, nux_u32_t slot, nux_u32_t n)
{
    return NUX_SUCCESS;
}
nux_u32_t
nux_os_read (void *userdata, nux_u32_t slot, void *p, nux_u32_t n)
{
    return 0;
}
void
nux_os_log (void           *userdata,
            nux_log_level_t level,
            const nux_c8_t *log,
            nux_u32_t       n)
{
    printf("%.*s\n", n, log);
}
nux_status_t
nux_os_create_pipeline (void                          *userdata,
                        nux_u32_t                      slot,
                        const nux_gpu_pipeline_info_t *info)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_framebuffer (void *userdata, nux_u32_t slot, nux_u32_t texture)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_texture (void                         *userdata,
                       nux_u32_t                     slot,
                       const nux_gpu_texture_info_t *info)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_update_texture (void       *userdata,
                       nux_u32_t   slot,
                       nux_u32_t   x,
                       nux_u32_t   y,
                       nux_u32_t   w,
                       nux_u32_t   h,
                       const void *data)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_create_buffer (void                 *userdata,
                      nux_u32_t             slot,
                      nux_gpu_buffer_type_t type,
                      nux_u32_t             size)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_os_update_buffer (void       *userdata,
                      nux_u32_t   slot,
                      nux_u32_t   offset,
                      nux_u32_t   size,
                      const void *data)
{
    return NUX_SUCCESS;
}
void
nux_os_gpu_submit (void                    *userdata,
                   const nux_gpu_command_t *cmds,
                   nux_u32_t                count)
{
}
void
nux_os_update_inputs (void *user, nux_u32_t *buttons, nux_f32_t *axis)
{
}
void
nux_os_update_stats (void *userdata, nux_u32_t *stats)
{
}

int
main (int argc, char *arv[])
{
    nux_config_t config;
    memset(&config, 0, sizeof(config));
    config.max_ref_count  = 4096;
    config.memory_size    = (1 << 24);
    config.width          = 800;
    config.height         = 400;
    config.tick_frequency = 60;
    config.userdata       = NULL;

    nux_ctx_t *ctx = nux_instance_init(&config);
    nux_instance_tick(ctx);
    nux_instance_free(ctx);

    return 0;
}
