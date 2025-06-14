#include "internal.h"

nux_u32_t
nux_texture_new (nux_env_t           *env,
                 nux_texture_format_t format,
                 nux_u32_t            w,
                 nux_u32_t            h)
{
    nux_frame_t frame = nux_begin_frame(env);

    // Create object
    nux_u32_t      id;
    nux_texture_t *tex = NUX_NEW(env, NUX_OBJECT_TEXTURE, nux_texture_t, &id);
    NUX_CHECKM(tex, "Failed to create texture object", return NUX_NULL);
    tex->format = format;
    tex->width  = w;
    tex->height = h;

    // Create gpu texture
    nux_gpu_texture_info_t info = {
        .format = format,
        .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
        .width  = w,
        .height = h,
    };
    nux_u32_t *slot = nux_u32_vec_pop(&env->inst->free_texture_slots);
    NUX_CHECKM(slot, "Out of gpu textures", goto cleanup);
    tex->slot = *slot;
    NUX_CHECKM(nux_os_create_texture(env->inst->userdata, tex->slot, &info),
               "Failed to create texture",
               goto cleanup);

    // Allocate memory
    nux_u32_t sample_size = 0;
    switch (format)
    {
        case NUX_TEXTURE_FORMAT_RGBA:
            sample_size = sizeof(nux_u32_t);
            break;
        case NUX_TEXTURE_FORMAT_INDEX:
            sample_size = sizeof(nux_u8_t);
            break;
    }
    NUX_CHECKM(sample_size, "Invalid texture format", goto cleanup);
    tex->data = nux_alloc(env, sample_size * w * h);
    NUX_CHECKM(tex->data, "Failed to allocate texture data", goto cleanup);
    nux_memset(tex->data, 0, sample_size * w * h);

    return id;

cleanup:
    nux_reset_frame(env, frame);
    return NUX_NULL;
}
void
nux_texture_cleanup (nux_env_t *env, void *data)
{
    nux_texture_t *tex = data;
    if (tex->slot)
    {
        *nux_u32_vec_push(&env->inst->free_texture_slots) = tex->slot;
    }
}
void
nux_texture_write (nux_env_t      *env,
                   nux_u32_t       id,
                   nux_u32_t       x,
                   nux_u32_t       y,
                   nux_u32_t       w,
                   nux_u32_t       h,
                   const nux_u8_t *data)
{
}
