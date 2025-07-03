#include "internal.h"

nux_u32_t
nux_texture_new (nux_ctx_t         *ctx,
                 nux_texture_type_t type,
                 nux_u32_t          w,
                 nux_u32_t          h)
{
    // Create object
    nux_texture_t *tex = nux_arena_alloc(ctx->active_arena, sizeof(*tex));
    NUX_CHECK(tex, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(ctx, ctx->active_arena, NUX_TYPE_TEXTURE, tex);
    NUX_CHECK(id, return NUX_NULL);
    tex->type   = type;
    tex->width  = w;
    tex->height = h;

    // Create gpu texture
    nux_gpu_texture_info_t info = {
        .type   = type,
        .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
        .width  = w,
        .height = h,
    };
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_texture_slots);
    NUX_CHECKM(slot, "Out of gpu textures", return NUX_NULL);
    tex->slot = *slot;
    NUX_CHECKM(nux_os_create_texture(ctx->userdata, tex->slot, &info),
               "Failed to create texture",
               return NUX_NULL);

    // Allocate memory
    nux_u32_t pixel_size;
    switch (type)
    {
        case NUX_TEXTURE_IMAGE_RGBA:
            pixel_size = sizeof(nux_u32_t);
            break;
        case NUX_TEXTURE_IMAGE_INDEX:
            pixel_size = sizeof(nux_u8_t);
            break;
        default:
            pixel_size = 0;
            break;
    }
    if (pixel_size)
    {
        tex->data = nux_arena_alloc(ctx->active_arena, pixel_size * w * h);
        NUX_CHECKM(
            tex->data, "Failed to allocate texture data", return NUX_NULL);
        nux_memset(tex->data, 0, pixel_size * w * h);
    }

    if (type == NUX_TEXTURE_RENDER_TARGET)
    {
        // Create framebuffer
        slot = nux_u32_vec_pop(&ctx->free_framebuffer_slots);
        NUX_CHECKM(slot, "Out of gpu framebuffer slots", return NUX_NULL);
        tex->framebuffer_slot = *slot;
        NUX_CHECKM(nux_os_create_framebuffer(
                       ctx->userdata, tex->framebuffer_slot, tex->slot),
                   "Failed to create framebuffer",
                   return NUX_NULL);
    }

    return id;
}
void
nux_texture_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_texture_t *tex = data;
    if (tex->slot)
    {
        nux_u32_vec_pushv(&ctx->free_texture_slots, tex->slot);
    }
    if (tex->framebuffer_slot)
    {
        nux_u32_vec_pushv(&ctx->free_framebuffer_slots, tex->framebuffer_slot);
    }
}
void
nux_texture_write (nux_ctx_t  *ctx,
                   nux_u32_t   id,
                   nux_u32_t   x,
                   nux_u32_t   y,
                   nux_u32_t   w,
                   nux_u32_t   h,
                   const void *data)
{
    nux_texture_t *tex = nux_object_get(ctx, NUX_TYPE_TEXTURE, id);
    NUX_CHECKM(tex, "Invalid texture id", return);
    NUX_CHECKM(tex->type != NUX_TEXTURE_RENDER_TARGET,
               "Trying to write render target texture",
               return);
    NUX_CHECKM(
        nux_os_update_texture(ctx->userdata, tex->slot, x, y, w, h, data),
        "Failed to update colormap texture",
        return);
}
