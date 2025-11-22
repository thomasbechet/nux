#include "internal.h"

#define STBIR_DEBUG
#define STB_IMAGE_STATIC
#define STBI_NO_STDIO
#define STBI_ASSERT(x) nux_assert(x)
#define STB_IMAGE_IMPLEMENTATION
#include <externals/stb/stb_image.h>

static nux_u32_t
pixel_size (const nux_texture_t *texture)
{
    nux_u32_t pixel_size;
    switch (texture->gpu.type)
    {
        case NUX_TEXTURE_IMAGE_RGBA:
            return sizeof(nux_u32_t);
        case NUX_TEXTURE_IMAGE_INDEX:
            return sizeof(nux_u8_t);
        default:
            return 0;
    }
}
static void
texture_update (nux_texture_t *tex,
                nux_u32_t      x,
                nux_u32_t      y,
                nux_u32_t      w,
                nux_u32_t      h,
                const void    *data)
{
    nux_ensure(tex->gpu.type != NUX_TEXTURE_RENDER_TARGET,
               return,
               "trying to write render target texture");
    nux_u32_t ps = pixel_size(tex);
    nux_assert(ps);
    if (x == 0 && y == 0 && w == tex->gpu.width && h == tex->gpu.height)
    {
        nux_memcpy(tex->data, data, w * h * ps);
    }
    else
    {
        nux_assert(0);
    }
    nux_b2i_t extent = nux_b2i(x, y, w, h);
    if (tex->dirty)
    {
        tex->dirty_extent = nux_b2i_merge(tex->dirty_extent, extent);
    }
    else
    {
        tex->dirty_extent = extent;
    }
    tex->dirty = true;
}

nux_texture_t *
nux_texture_new (nux_arena_t       *arena,
                 nux_texture_type_t type,
                 nux_u32_t          w,
                 nux_u32_t          h)
{
    // Create object
    nux_texture_t *tex = nux_object_new(arena, NUX_OBJECT_TEXTURE);
    nux_check(tex, return nullptr);
    tex->gpu.type     = type;
    tex->gpu.width    = w;
    tex->gpu.height   = h;
    tex->dirty        = type != NUX_TEXTURE_RENDER_TARGET;
    tex->dirty_extent = nux_b2i(0, 0, w, h);

    // Create gpu texture (and framebuffer if needed)
    nux_check(nux_gpu_texture_init(&tex->gpu), return nullptr);

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
        tex->data = nux_malloc(arena, pixel_size * w * h);
        nux_check(tex->data, return nullptr);
        nux_memset(tex->data, 0, pixel_size * w * h);
    }

    return tex;
}
nux_texture_t *
nux_texture_load (nux_arena_t *arena, const nux_c8_t *path)
{
    nux_u32_t size;
    void     *data = nux_file_load(nux_arena_frame(), path, &size);
    nux_check(data, return nullptr);
    return nux_texture_load_from_memory(arena, data, size);
}
nux_texture_t *
nux_texture_load_from_memory (nux_arena_t    *arena,
                              const nux_u8_t *data,
                              nux_u32_t       size)
{
    nux_i32_t w, h, n;
    nux_u8_t *img
        = stbi_load_from_memory(data, size, &w, &h, &n, STBI_rgb_alpha);
    nux_texture_t *tex = nux_texture_new(arena, NUX_TEXTURE_IMAGE_RGBA, w, h);
    nux_check(tex, goto error);
    texture_update(tex, 0, 0, w, h, img);
error:
    stbi_image_free(img);
    return tex;
}
void
nux_texture_cleanup (void *data)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_texture_t         *tex = data;
    if (tex->gpu.slot)
    {
        nux_vec_pushv(&gfx->free_texture_slots, tex->gpu.slot);
    }
    if (tex->gpu.framebuffer_slot)
    {
        nux_vec_pushv(&gfx->free_framebuffer_slots, tex->gpu.framebuffer_slot);
    }
}
nux_texture_t *
nux_texture_screen (void)
{
    return nux_graphics()->screen_target;
}
nux_v2i_t
nux_texture_size (nux_texture_t *texture)
{
    return nux_v2i(texture->gpu.width, texture->gpu.height);
}
void
nux_texture_blit (nux_texture_t *tex, nux_texture_t *target, nux_b2i_t extent)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_check(tex->gpu.type == NUX_TEXTURE_RENDER_TARGET, return);
    nux_gpu_encoder_t enc;
    nux_gpu_encoder_init(nux_arena_frame(), &enc);
    nux_gpu_bind_framebuffer(&enc,
                             target ? target->gpu.framebuffer_slot
                                    : NUX_GPU_WINDOW_FRAMEBUFFER);
    nux_gpu_viewport(&enc, extent);
    nux_gpu_bind_pipeline(&enc, gfx->blit_pipeline.slot);
    nux_gpu_bind_texture(&enc, NUX_GPU_DESC_BLIT_TEXTURE, tex->gpu.slot);
    nux_gpu_push_u32(&enc, NUX_GPU_DESC_BLIT_TEXTURE_WIDTH, tex->gpu.width);
    nux_gpu_push_u32(&enc, NUX_GPU_DESC_BLIT_TEXTURE_HEIGHT, tex->gpu.height);
    nux_gpu_draw_full_quad(&enc);
    nux_gpu_encoder_submit(&enc);
}
nux_status_t
nux_texture_upload (nux_texture_t *texture)
{
    nux_ensure(texture->gpu.type != NUX_TEXTURE_RENDER_TARGET,
               return NUX_FAILURE,
               "trying to write render target texture");
    if (texture->dirty)
    {
        nux_ensure(nux_os_texture_update(texture->gpu.slot,
                                         texture->dirty_extent.x,
                                         texture->dirty_extent.y,
                                         texture->dirty_extent.w,
                                         texture->dirty_extent.h,
                                         texture->data),
                   return NUX_FAILURE,
                   "failed to update colormap texture");
        texture->dirty = false;
    }
    return NUX_SUCCESS;
}
