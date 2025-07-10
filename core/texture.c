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
    tex->gpu.type   = type;
    tex->gpu.width  = w;
    tex->gpu.height = h;

    // Create gpu texture
    NUX_CHECK(nux_gpu_texture_init(ctx, &tex->gpu), return NUX_NULL);

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

    return id;
}
void
nux_texture_cleanup (nux_ctx_t *ctx, void *data)
{
    nux_texture_t *tex = data;
    if (tex->gpu.slot)
    {
        nux_u32_vec_pushv(&ctx->free_texture_slots, tex->gpu.slot);
    }
    if (tex->gpu.framebuffer_slot)
    {
        nux_u32_vec_pushv(&ctx->free_framebuffer_slots,
                          tex->gpu.framebuffer_slot);
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
    NUX_CHECKM(tex->gpu.type != NUX_TEXTURE_RENDER_TARGET,
               "Trying to write render target texture",
               return);
    NUX_CHECKM(
        nux_os_update_texture(ctx->userdata, tex->gpu.slot, x, y, w, h, data),
        "Failed to update colormap texture",
        return);
}
void
nux_texture_blit (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_texture_t *tex = nux_object_get(ctx, NUX_TYPE_TEXTURE, id);
    NUX_CHECK(tex, return);
    NUX_CHECK(tex->gpu.type == NUX_TEXTURE_RENDER_TARGET, return);
    nux_gpu_command_t     cmds_data[10];
    nux_gpu_command_vec_t cmds;
    nux_gpu_command_vec_init(cmds_data, NUX_ARRAY_SIZE(cmds_data), &cmds);
    nux_gpu_bind_framebuffer(&cmds, 0);
    nux_gpu_bind_pipeline(&cmds, ctx->blit_pipeline.slot);
    nux_gpu_bind_texture(&cmds, NUX_GPU_DESC_BLIT_TEXTURE, tex->gpu.slot);
    nux_gpu_push_u32(&cmds, NUX_GPU_DESC_BLIT_TEXTURE_WIDTH, tex->gpu.width);
    nux_gpu_push_u32(&cmds, NUX_GPU_DESC_BLIT_TEXTURE_HEIGHT, tex->gpu.height);
    nux_gpu_draw(&cmds, 3);
    nux_os_gpu_submit(ctx->userdata, cmds.data, cmds.size);
}
