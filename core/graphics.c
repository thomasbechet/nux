#include "internal.h"

#include "fonts_data.c.inc"

#define VERTEX_SIZE             5
#define VERTICES_DEFAULT_SIZE   (1 << 18) // 32k
#define TRANSFORMS_DEFAULT_SIZE 4096

nux_status_t
nux_graphics_init (nux_ctx_t *ctx)
{
    // Initialize gpu slots
    NUX_CHECKM(nux_u32_vec_alloc(ctx->core_arena,
                                 NUX_GPU_PIPELINE_MAX,
                                 &ctx->free_pipeline_slots),
               "Failed to allocate gpu pipeline slots",
               goto error);
    NUX_CHECKM(nux_u32_vec_alloc(ctx->core_arena,
                                 NUX_GPU_TEXTURE_MAX,
                                 &ctx->free_texture_slots),
               "Failed to allocate gpu texture slots",
               goto error);
    NUX_CHECKM(nux_u32_vec_alloc(ctx->core_arena,
                                 NUX_GPU_FRAMEBUFFER_MAX,
                                 &ctx->free_framebuffer_slots),
               "Failed to allocate gpu framebuffer slots",
               goto error);
    NUX_CHECKM(nux_u32_vec_alloc(ctx->core_arena,
                                 NUX_GPU_BUFFER_MAX,
                                 &ctx->free_buffer_slots),
               "Failed to allocate gpu buffer slots",
               goto error);

    nux_u32_vec_fill_reversed(&ctx->free_pipeline_slots);
    nux_u32_vec_fill_reversed(&ctx->free_texture_slots);
    nux_u32_vec_fill_reversed(&ctx->free_framebuffer_slots);
    nux_u32_vec_fill_reversed(&ctx->free_buffer_slots);

    // Allocate gpu commands buffer
    NUX_CHECKM(
        nux_gpu_command_vec_alloc(ctx->core_arena, 4096, &ctx->gpu_commands),
        "Failed to allocate gpu commands buffer",
        goto error);

    // Initialize state
    nux_palr(ctx);

    // Create pipelines
    ctx->main_pipeline.type = NUX_GPU_PIPELINE_MAIN;
    NUX_CHECKM(nux_gpu_pipeline_init(ctx, &ctx->main_pipeline),
               "Failed to create main pipeline",
               goto error);
    ctx->blit_pipeline.type = NUX_GPU_PIPELINE_BLIT;
    NUX_CHECKM(nux_gpu_pipeline_init(ctx, &ctx->blit_pipeline),
               "Failed to create blit pipeline",
               goto error);

    // Create buffers
    ctx->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    ctx->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECKM(nux_gpu_buffer_init(ctx, &ctx->constants_buffer),
               "Failed to create constants buffer",
               goto error);

    ctx->vertices_buffer.type = NUX_GPU_BUFFER_STORAGE;
    ctx->vertices_buffer.size = VERTEX_SIZE * VERTICES_DEFAULT_SIZE;
    ctx->vertices_buffer_head = 0;
    NUX_CHECKM(nux_gpu_buffer_init(ctx, &ctx->vertices_buffer),
               "Failed to create vertices buffer",
               goto error);

    ctx->transforms_buffer_head = 0;
    ctx->transforms_buffer.type = NUX_GPU_BUFFER_STORAGE;
    ctx->transforms_buffer.size = NUX_M4_SIZE * TRANSFORMS_DEFAULT_SIZE;
    NUX_CHECKM(nux_gpu_buffer_init(ctx, &ctx->transforms_buffer),
               "Failed to create transforms buffer",
               goto error);

    // Create canvas
    NUX_CHECKM(nux_canvas_init(ctx, &ctx->canvas),
               "Failed to initialize canvas",
               goto error);

    return NUX_SUCCESS;

error:
    return NUX_FAILURE;
}
nux_status_t
nux_graphics_free (nux_ctx_t *ctx)
{
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_render (nux_ctx_t *ctx)
{
    // Submit canvas commands
    nux_canvas_end(ctx, &ctx->canvas);
    nux_canvas_begin(ctx, &ctx->canvas);

    // Reset frame data (TODO: mode to scene)
    ctx->transforms_buffer_head = 0;
    nux_gpu_command_vec_clear(&ctx->gpu_commands);

    return NUX_SUCCESS;
}

nux_status_t
nux_graphics_push_vertices (nux_ctx_t       *ctx,
                            nux_u32_t        vcount,
                            const nux_f32_t *data,
                            nux_u32_t       *first)
{
    NUX_CHECKM(ctx->vertices_buffer_head + vcount < VERTICES_DEFAULT_SIZE,
               "Out of vertices",
               return NUX_FAILURE);
    NUX_CHECKM(nux_os_update_buffer(ctx->userdata,
                                    ctx->vertices_buffer.slot,
                                    ctx->vertices_buffer_head * VERTEX_SIZE
                                        * sizeof(nux_f32_t),
                                    vcount * VERTEX_SIZE * sizeof(nux_f32_t),
                                    data),
               "Failed to update vertex buffer",
               return NUX_FAILURE);
    *first = ctx->vertices_buffer_head;
    ctx->vertices_buffer_head += vcount;
    return NUX_SUCCESS;
}
nux_status_t
nux_graphics_push_transforms (nux_ctx_t      *ctx,
                              nux_u32_t       mcount,
                              const nux_m4_t *data,
                              nux_u32_t      *index)
{
    NUX_CHECKM(ctx->transforms_buffer_head + mcount < TRANSFORMS_DEFAULT_SIZE,
               "Out of transforms",
               return NUX_FAILURE);
    NUX_CHECKM(nux_os_update_buffer(ctx->userdata,
                                    ctx->transforms_buffer.slot,
                                    ctx->transforms_buffer_head * NUX_M4_SIZE
                                        * sizeof(nux_f32_t),
                                    mcount * NUX_M4_SIZE * sizeof(nux_f32_t),
                                    data),
               "Failed to update transform buffer",
               return NUX_FAILURE);
    *index = ctx->transforms_buffer_head;
    ctx->transforms_buffer_head += mcount;
    return NUX_SUCCESS;
}

void
nux_pal (nux_ctx_t *ctx, nux_u8_t index, nux_u8_t color)
{
    ctx->pal[index] = color;
}
void
nux_palt (nux_ctx_t *ctx, nux_u8_t c)
{
}
void
nux_palr (nux_ctx_t *ctx)
{
    for (nux_u32_t i = 0; i < NUX_PALETTE_SIZE; ++i)
    {
        ctx->pal[i] = i;
    }
    nux_palt(ctx, 0);
}
nux_u8_t
nux_palc (nux_ctx_t *ctx, nux_u8_t index)
{
    return ctx->pal[index];
}
void
nux_cls (nux_ctx_t *ctx, nux_u32_t color)
{
    // nux_rectfill(ctx, 0, 0, NUX_CANVAS_WIDTH - 1, NUX_CANVAS_HEIGHT - 1,
    // color);
}
void
nux_graphics_text (
    nux_ctx_t *ctx, nux_i32_t x, nux_i32_t y, const nux_c8_t *text, nux_u8_t c)
{
    const nux_u32_t pixel_per_glyph
        = DEFAULT_FONT_DATA_WIDTH * DEFAULT_FONT_DATA_HEIGHT;

    nux_u32_t len = nux_strnlen(text, 1024);
    nux_b2i_t extent
        = nux_b2i_xywh(x, y, DEFAULT_FONT_DATA_WIDTH, DEFAULT_FONT_DATA_HEIGHT);
    const nux_c8_t *end = text + len;
    const nux_c8_t *it  = text;
    for (; it < end; ++it)
    {
        if (*it == '\n')
        {
            extent = nux_b2i_moveto(
                extent, nux_v2i(x, extent.min.y + DEFAULT_FONT_DATA_HEIGHT));
            continue;
        }
        nux_u32_t index = default_font_data_chars[(nux_u8_t)(*it)];

        for (nux_u32_t p = 0; p < pixel_per_glyph; ++p)
        {
            nux_u32_t bit_offset = index * pixel_per_glyph + p;
            NUX_ASSERT((bit_offset / 8) < sizeof(default_font_data));
            nux_u8_t  byte    = default_font_data[bit_offset / 8];
            nux_b32_t bit_set = (byte & (1 << (7 - (p % 8)))) != 0;

            nux_i32_t px = extent.min.x + p % DEFAULT_FONT_DATA_WIDTH;
            nux_i32_t py = extent.min.y + p / DEFAULT_FONT_DATA_WIDTH;

            if (bit_set)
            {
                nux_pset(ctx, px, py, c);

                // Apply shadow
                nux_pset(ctx, px + 1, py + 1, 0);
            }
        }

        extent = nux_b2i_translate(extent, nux_v2i(DEFAULT_FONT_DATA_WIDTH, 0));
    }
}
void
nux_graphics_print (nux_ctx_t *ctx, const nux_c8_t *text, nux_u8_t c)
{
    nux_i32_t x = nux_graphics_cursor_x(ctx);
    nux_i32_t y = nux_graphics_cursor_y(ctx);
    nux_graphics_text(ctx, x, y, text, c);
    nux_graphics_cursor(ctx, x, y + DEFAULT_FONT_DATA_HEIGHT);
}
#ifdef NUX_BUILD_VARARGS
void
nux_textfmt (nux_ctx_t      *ctx,
             nux_i32_t       x,
             nux_i32_t       y,
             nux_u8_t        c,
             const nux_c8_t *fmt,
             ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_graphics_text(ctx, x, y, buf, c);
}
void
nux_printfmt (nux_ctx_t *ctx, nux_u8_t c, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_graphics_print(ctx, buf, c);
}
void
nux_tracefmt (nux_ctx_t *ctx, const nux_c8_t *fmt, ...)
{
    nux_c8_t buf[128];
    va_list  args;
    va_start(args, fmt);
    nux_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    nux_trace(ctx, buf);
}
#endif

nux_i32_t
nux_graphics_cursor_x (nux_ctx_t *ctx)
{
    return ctx->cursor.x;
}
nux_i32_t
nux_graphics_cursor_y (nux_ctx_t *ctx)
{
    return ctx->cursor.y;
}
void
nux_graphics_cursor (nux_ctx_t *ctx, nux_i32_t x, nux_i32_t y)
{
    ctx->cursor = nux_v2i(x, y);
}
nux_u32_t
nux_cget (nux_ctx_t *ctx, nux_u8_t index)
{
    const nux_u8_t *map = (const nux_u8_t *)ctx->colormap;
    return (map[index * 3 + 0] << 16 | map[index * 3 + 1] << 8
            | map[index * 3 + 0]);
}
void
nux_cset (nux_ctx_t *ctx, nux_u8_t index, nux_u32_t color)
{
    nux_u8_t *map      = (nux_u8_t *)ctx->colormap;
    map[index * 3 + 0] = (color & 0xFF0000) >> 16;
    map[index * 3 + 1] = (color & 0xFF00) >> 8;
    map[index * 3 + 2] = color & 0xFF;
}

void
nux_write_texture (nux_ctx_t      *ctx,
                   nux_u32_t       x,
                   nux_u32_t       y,
                   nux_u32_t       w,
                   nux_u32_t       h,
                   const nux_u8_t *data)
{
    // Clamp to region
    x = NUX_MIN(x, NUX_TEXTURE_WIDTH - 1);
    y = NUX_MIN(y, NUX_TEXTURE_HEIGHT - 1);
    w = NUX_MIN(w, NUX_TEXTURE_WIDTH - x - 1);
    h = NUX_MIN(h, NUX_TEXTURE_HEIGHT - y - 1);

    // Copy row by row
    // nux_u8_t *tex = NUX_MEMPTR(ctx->NUX_RAM_TEXTURE, nux_u8_t);
    // for (nux_u32_t i = 0; i < h; ++i)
    // {
    //     nux_u8_t       *dst = tex + ((y + i) * NUX_TEXTURE_WIDTH + x);
    //     const nux_u8_t *src = data + (i * w);
    //     nux_memcpy(dst, src, w);
    // }
}

void
nux_graphics_set_render_target (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_texture_t *tex = nux_object_get(ctx, NUX_TYPE_TEXTURE, id);
    NUX_CHECKM(tex->type == NUX_TEXTURE_RENDER_TARGET,
               "Texture is not a render target",
               return);
}
void
nux_graphics_blit (nux_ctx_t *ctx, nux_u32_t id)
{
}
