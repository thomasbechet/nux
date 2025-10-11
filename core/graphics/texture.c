#include "internal.h"

nux_texture_t *
nux_texture_new (nux_arena_t       *arena,
                 nux_texture_type_t type,
                 nux_u32_t          w,
                 nux_u32_t          h)
{
    // Create object
    nux_texture_t *tex = nux_resource_new(arena, NUX_RESOURCE_TEXTURE);
    NUX_CHECK(tex, return NUX_NULL);
    tex->gpu.type   = type;
    tex->gpu.width  = w;
    tex->gpu.height = h;

    // Create gpu texture
    NUX_CHECK(nux_gpu_texture_init(&tex->gpu), return NUX_NULL);

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
        tex->data = nux_arena_malloc(arena, pixel_size * w * h);
        NUX_CHECK(tex->data, return NUX_NULL);
        nux_memset(tex->data, 0, pixel_size * w * h);
    }

    return tex;
}
void
nux_texture_cleanup (void *data)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_texture_t         *tex    = data;
    if (tex->gpu.slot)
    {
        nux_u32_vec_pushv(&module->free_texture_slots, tex->gpu.slot);
    }
    if (tex->gpu.framebuffer_slot)
    {
        nux_u32_vec_pushv(&module->free_framebuffer_slots,
                          tex->gpu.framebuffer_slot);
    }
}
void
nux_texture_write (nux_texture_t *tex,
                   nux_u32_t      x,
                   nux_u32_t      y,
                   nux_u32_t      w,
                   nux_u32_t      h,
                   const void    *data)
{
    NUX_ENSURE(tex->gpu.type != NUX_TEXTURE_RENDER_TARGET,
               return,
               "trying to write render target texture");
    NUX_ENSURE(
        nux_os_texture_update(nux_userdata(), tex->gpu.slot, x, y, w, h, data),
        return,
        "failed to update colormap texture");
}
void
nux_texture_blit (nux_texture_t *tex, nux_u32_t framebuffer)
{
    nux_graphics_module_t *module = nux_graphics_module();
    NUX_CHECK(tex->gpu.type == NUX_TEXTURE_RENDER_TARGET, return);
    nux_gpu_encoder_t enc;
    nux_gpu_encoder_init(nux_arena_frame(), &enc);
    nux_gpu_bind_framebuffer(&enc, framebuffer);
    nux_gpu_bind_pipeline(&enc, module->blit_pipeline.slot);
    nux_gpu_bind_texture(&enc, NUX_GPU_DESC_BLIT_TEXTURE, tex->gpu.slot);
    nux_gpu_push_u32(&enc, NUX_GPU_DESC_BLIT_TEXTURE_WIDTH, tex->gpu.width);
    nux_gpu_push_u32(&enc, NUX_GPU_DESC_BLIT_TEXTURE_HEIGHT, tex->gpu.height);
    nux_gpu_draw(&enc, 3);
    nux_gpu_encoder_submit(&enc);
}
