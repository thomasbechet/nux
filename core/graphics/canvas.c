#include "internal.h"

static nux_gpu_canvas_quad_t
make_quad (nux_u16_t x,
           nux_u16_t y,
           nux_u16_t tx,
           nux_u16_t ty,
           nux_u16_t sx,
           nux_u16_t sy)
{
    nux_gpu_canvas_quad_t q;
    q.pos  = ((nux_u32_t)y << 16) | x;
    q.tex  = ((nux_u32_t)ty << 16) | tx;
    q.size = ((nux_u32_t)sy << 16) | sx;
    // blit->depth
    //     = (pass->depth - NUGL__MIN_DEPTH) / (NUGL__MAX_DEPTH -
    //     NUGL__MIN_DEPTH);
    return q;
}
static void
begin_batch_textured (nux_canvas_t *canvas,
                      nux_u32_t     texture_slot,
                      nux_u32_t     texture_width,
                      nux_u32_t     texture_height,
                      nux_v4_t      color)
{
    canvas->active_batch.mode           = 1;
    canvas->active_batch.first          = canvas->quads_buffer_head;
    canvas->active_batch.count          = 0;
    canvas->active_batch.texture_width  = texture_width;
    canvas->active_batch.texture_height = texture_height;
    canvas->active_batch.color          = color;
    canvas->active_texture              = texture_slot;
}
static void
begin_batch_colored (nux_canvas_t *canvas, nux_v4_t color)
{
    canvas->active_batch.mode           = 0;
    canvas->active_batch.first          = canvas->quads_buffer_head;
    canvas->active_batch.count          = 0;
    canvas->active_batch.texture_width  = 0;
    canvas->active_batch.texture_height = 0;
    canvas->active_batch.color          = color;
    canvas->active_texture              = 0;
}
static nux_status_t
end_batch (nux_canvas_t *canvas)
{
    // Update buffer
    nux_u32_t index = canvas->batches_buffer_head;
    NUX_ENSURE(nux_os_buffer_update(nux_userdata(),
                                    canvas->batches_buffer.slot,
                                    index * sizeof(canvas->active_batch),
                                    sizeof(canvas->active_batch),
                                    &canvas->active_batch),
               return NUX_FAILURE,
               "failed to update batches buffer");
    ++canvas->batches_buffer_head;

    // Build commands
    nux_gpu_bind_texture(
        &canvas->encoder, NUX_GPU_DESC_CANVAS_TEXTURE, canvas->active_texture);
    nux_gpu_push_u32(&canvas->encoder, NUX_GPU_DESC_CANVAS_BATCH_INDEX, index);
    nux_gpu_draw(&canvas->encoder, canvas->active_batch.count * 6);

    return NUX_SUCCESS;
}
static nux_status_t
push_quads (nux_canvas_t *canvas, nux_gpu_canvas_quad_t *quads, nux_u32_t count)
{
    NUX_ENSURE(canvas->quads_buffer_head + count < NUX_QUADS_DEFAULT_SIZE,
               return NUX_FAILURE,
               "out of quads");
    NUX_ENSURE(nux_os_buffer_update(nux_userdata(),
                                    canvas->quads_buffer.slot,
                                    canvas->quads_buffer_head * sizeof(*quads),
                                    count * sizeof(*quads),
                                    quads),
               return NUX_FAILURE,
               "failed to update quads buffer");
    canvas->active_batch.count += count;
    canvas->quads_buffer_head += count;
    return NUX_SUCCESS;
}

nux_canvas_t *
nux_canvas_new (nux_arena_t *arena, nux_u32_t width, nux_u32_t height)
{
    nux_canvas_t *c = nux_resource_new(arena, NUX_RESOURCE_CANVAS);
    NUX_CHECK(c, return NUX_NULL);

    // Allocate constants buffer
    c->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    c->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECK(nux_gpu_buffer_init(&c->constants_buffer), return NUX_NULL);

    // Allocate quads buffer
    c->quads_buffer_head = 0;
    c->quads_buffer.type = NUX_GPU_BUFFER_STORAGE;
    c->quads_buffer.size
        = sizeof(nux_gpu_canvas_quad_t) * NUX_QUADS_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(&c->quads_buffer), return NUX_NULL);

    // Allocate batches buffer
    c->batches_buffer_head = 0;
    c->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    c->batches_buffer.size
        = sizeof(nux_gpu_canvas_batch_t) * NUX_BATCHES_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(&c->batches_buffer), return NUX_NULL);

    // Allocate commands
    NUX_CHECK(nux_gpu_encoder_init(nux_arena_core(), &c->encoder),
              return NUX_NULL);

    // Initialize base active batch
    c->active_batch.mode  = 0;
    c->active_batch.first = 0;
    c->active_batch.count = 0;
    c->active_texture     = 0;
    c->wrap_mode          = NUX_TEXTURE_WRAP_REPEAT;

    // Create render target
    c->target
        = nux_texture_new(arena, NUX_TEXTURE_RENDER_TARGET, width, height);
    NUX_CHECK(c->target, return NUX_NULL);

    return c;
}
void
nux_canvas_cleanup (void *data)
{
    nux_canvas_t *canvas = data;
    nux_gpu_buffer_free(&canvas->constants_buffer);
    nux_gpu_buffer_free(&canvas->batches_buffer);
    nux_gpu_buffer_free(&canvas->quads_buffer);
}
nux_texture_t *
nux_canvas_get_texture (nux_canvas_t *canvas)
{
    return canvas->target;
}
void
nux_canvas_set_clear_color (nux_canvas_t *canvas, nux_u32_t color)
{
    canvas->clear_color = color;
}
void
nux_canvas_set_wrap_mode (nux_canvas_t *canvas, nux_texture_wrap_mode_t mode)
{
    canvas->wrap_mode = mode;
}
void
nux_canvas_text (nux_canvas_t   *canvas,
                 nux_u32_t       x,
                 nux_u32_t       y,
                 const nux_c8_t *text)
{
    nux_graphics_module_t *module = nux_graphics_module();
    nux_font_t            *font   = &module->default_font;

    begin_batch_textured(canvas,
                         font->texture.slot,
                         font->texture.width,
                         font->texture.height,
                         nux_v4s(1));

    nux_u32_t len = nux_strnlen(text, 1024);
    nux_b2i_t extent
        = nux_b2i_xywh(x, y, font->glyph_width, font->glyph_height);
    const nux_c8_t *it = text;
    while (*it != '\0')
    {
        // Check next line
        if (*it == '\n')
        {
            extent = nux_b2i_moveto(
                extent, nux_v2i(x, extent.min.y + font->glyph_height));
            ++it;
            continue;
        }

        // Generate quad
        nux_u32_t             index = font->char_to_glyph_index[(nux_u8_t)*it];
        nux_gpu_canvas_quad_t quad  = make_quad(extent.min.x,
                                               extent.min.y,
                                               index * font->glyph_width,
                                               0,
                                               font->glyph_width,
                                               font->glyph_height);
        NUX_CHECK(push_quads(canvas, &quad, 1), return);

        // Move to next character
        extent = nux_b2i_translate(extent, nux_v2i(font->glyph_width - 1, 0));
        ++it;
    }

    // Render quads
    end_batch(canvas);
}
void
nux_canvas_rectangle (
    nux_canvas_t *canvas, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    nux_gpu_canvas_quad_t quad = make_quad(x, y, 0, 0, w, h);

    begin_batch_colored(canvas, nux_v4(0.5, 1, 0.3, 1));
    nux_u32_t first;
    NUX_CHECK(push_quads(canvas, &quad, 1), return);
    end_batch(canvas);
}
void
nux_canvas_blit (nux_canvas_t  *canvas,
                 nux_texture_t *texture,
                 nux_b2i_t      extent,
                 nux_b2i_t      tex_extent)
{
    begin_batch_textured(canvas,
                         texture->gpu.slot,
                         texture->gpu.width,
                         texture->gpu.height,
                         nux_v4s(1));

    nux_gpu_canvas_quad_t quads[6];
    nux_u32_t             quads_count = 0;

    nux_v2u_t extent_s     = nux_b2i_size(extent);
    nux_v2u_t tex_extent_s = nux_b2i_size(tex_extent);

    switch (canvas->wrap_mode)
    {
        case NUX_TEXTURE_WRAP_CLAMP: {
            quads[0]    = make_quad(extent.min.x,
                                 extent.min.y,
                                 tex_extent.min.x,
                                 tex_extent.min.y,
                                 NUX_MIN(extent_s.x, tex_extent_s.x),
                                 NUX_MIN(extent_s.y, tex_extent_s.y));
            quads_count = 1;
        }
        break;
        case NUX_TEXTURE_WRAP_REPEAT: {
            // Calculate blit count
            nux_u32_t full_hblit_count = extent_s.x / tex_extent_s.x;
            nux_u32_t full_vblit_count = extent_s.y / tex_extent_s.y;

            // Insert full blits
            for (nux_u32_t y = 0; y < full_vblit_count; ++y)
            {
                for (nux_u32_t x = 0; x < full_hblit_count; ++x)
                {
                    nux_i32_t pos_x    = extent.min.x + (x * tex_extent_s.x);
                    nux_i32_t pos_y    = extent.min.y + (y * tex_extent_s.y);
                    quads[quads_count] = make_quad(pos_x,
                                                   pos_y,
                                                   tex_extent.min.x,
                                                   tex_extent.min.y,
                                                   tex_extent_s.x,
                                                   tex_extent_s.y);
                    ++quads_count;
                }
            }

            // Insert partial blits
            nux_u32_t partial_hblit_size = extent_s.x % tex_extent_s.x;
            nux_u32_t partial_vblit_size = extent_s.y % tex_extent_s.y;

            if (partial_hblit_size)
            {
                for (nux_u32_t y = 0; y < full_vblit_count; ++y)
                {
                    nux_i32_t pos_x
                        = extent.min.x + (full_hblit_count * tex_extent_s.x);
                    nux_i32_t pos_y = extent.min.y + (y * tex_extent_s.y);
                    nux_v2u_t size
                        = nux_v2u(partial_hblit_size, tex_extent_s.y);
                    quads[quads_count] = make_quad(pos_x,
                                                   pos_y,
                                                   tex_extent.min.x,
                                                   tex_extent.min.y,
                                                   size.x,
                                                   size.y);
                    ++quads_count;
                }
            }
            if (partial_vblit_size)
            {
                for (nux_u32_t x = 0; x < full_hblit_count; ++x)
                {
                    nux_i32_t pos_x = extent.min.x + (x * tex_extent_s.x);
                    nux_i32_t pos_y
                        = extent.min.y + (full_vblit_count * tex_extent_s.y);
                    nux_v2u_t size
                        = nux_v2u(tex_extent_s.x, partial_vblit_size);
                    quads[quads_count] = make_quad(pos_x,
                                                   pos_y,
                                                   tex_extent.min.x,
                                                   tex_extent.min.y,
                                                   size.x,
                                                   size.y);
                    ++quads_count;
                }
            }
            if (partial_hblit_size && partial_vblit_size)
            {
                nux_i32_t pos_x
                    = extent.min.x + (full_hblit_count * tex_extent_s.x);
                nux_i32_t pos_y
                    = extent.min.y + (full_vblit_count * tex_extent_s.y);
                nux_v2u_t size
                    = nux_v2u(partial_hblit_size, partial_vblit_size);
                quads[quads_count] = make_quad(pos_x,
                                               pos_y,
                                               tex_extent.min.x,
                                               tex_extent.min.y,
                                               size.x,
                                               size.y);
                ++quads_count;
            }
        }
        break;
        case NUX_TEXTURE_WRAP_MIRROR:
            break;
    }

    nux_u32_t first;
    NUX_CHECK(push_quads(canvas, quads, quads_count), return);
    end_batch(canvas);
}
void
nux_canvas_blit_sliced (nux_canvas_t  *canvas,
                        nux_texture_t *texture,
                        nux_b2i_t      extent,
                        nux_b2i_t      tex_extent,
                        nux_b2i_t      inner)
{
    nux_u32_t margin_left   = inner.min.x - tex_extent.min.x;
    nux_u32_t margin_right  = tex_extent.max.x - inner.max.x;
    nux_u32_t margin_top    = inner.min.y - tex_extent.min.y;
    nux_u32_t margin_bottom = tex_extent.max.y - inner.max.y;

    nux_u32_t tex_mid_width
        = nux_b2i_size(tex_extent).x - margin_left - margin_right;
    nux_u32_t tex_mid_height
        = nux_b2i_size(tex_extent).y - margin_top - margin_bottom;
    nux_u32_t ext_mid_width
        = nux_b2i_size(extent).x - margin_left - margin_right;
    nux_u32_t ext_mid_height
        = nux_b2i_size(extent).y - margin_top - margin_bottom;

    // Top-Left
    if (margin_top && margin_left)
    {
        nux_canvas_blit(
            canvas,
            texture,
            nux_b2i_xywh(extent.min.x, extent.min.y, margin_left, margin_top),
            nux_b2i_xywh(
                tex_extent.min.x, tex_extent.min.y, margin_left, margin_top));
    }

    // Top-Mid
    if (margin_top)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.min.x + margin_left,
                                     extent.min.y,
                                     ext_mid_width,
                                     margin_top),
                        nux_b2i_xywh(tex_extent.min.x + margin_left,
                                     tex_extent.min.y,
                                     tex_mid_width,
                                     margin_top));
    }

    // Top-Right
    if (margin_top && margin_right)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.max.x - margin_right + 1,
                                     extent.min.y,
                                     margin_right,
                                     margin_top),
                        nux_b2i_xywh(tex_extent.max.x - margin_right + 1,
                                     tex_extent.min.y,
                                     margin_right,
                                     margin_top));
    }

    // Mid-Left
    if (margin_left)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.min.x,
                                     extent.min.y + margin_top,
                                     margin_right,
                                     ext_mid_height),
                        nux_b2i_xywh(tex_extent.min.x,
                                     tex_extent.min.y + margin_top,
                                     margin_right,
                                     tex_mid_height));
    }

    // Mid-Mid
    nux_canvas_blit(canvas,
                    texture,
                    nux_b2i_xywh(extent.min.x + margin_left,
                                 extent.min.y + margin_top,
                                 ext_mid_width,
                                 ext_mid_height),
                    nux_b2i_xywh(tex_extent.min.x + margin_left,
                                 tex_extent.min.y + margin_top,
                                 tex_mid_width,
                                 tex_mid_height));

    // Mid-Right
    if (margin_right)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.max.x - margin_right + 1,
                                     extent.min.y + margin_top,
                                     margin_right,
                                     ext_mid_height),
                        nux_b2i_xywh(tex_extent.max.x - margin_right + 1,
                                     tex_extent.min.y + margin_top,
                                     margin_right,
                                     tex_mid_height));
    }

    // Bottom-Left
    if (margin_bottom && margin_left)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.min.x,
                                     extent.max.y - margin_bottom + 1,
                                     margin_left,
                                     margin_bottom),
                        nux_b2i_xywh(tex_extent.min.x,
                                     tex_extent.max.y - margin_bottom + 1,
                                     margin_right,
                                     margin_bottom));
    }

    // Bottom-Mid
    if (margin_bottom)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.min.x + margin_left,
                                     extent.max.y - margin_bottom + 1,
                                     ext_mid_width,
                                     margin_bottom),
                        nux_b2i_xywh(tex_extent.min.x + margin_left,
                                     tex_extent.max.y - margin_bottom + 1,
                                     tex_mid_width,
                                     margin_bottom));
    }

    // Bottom-Right
    if (margin_bottom && margin_right)
    {
        nux_canvas_blit(canvas,
                        texture,
                        nux_b2i_xywh(extent.max.x - margin_right + 1,
                                     extent.max.y - margin_bottom + 1,
                                     margin_right,
                                     margin_bottom),
                        nux_b2i_xywh(tex_extent.max.x - margin_right + 1,
                                     tex_extent.max.y - margin_bottom + 1,
                                     margin_right,
                                     margin_bottom));
    }
}

void
nux_canvas_render (nux_canvas_t *c)
{
    nux_graphics_module_t *module = nux_graphics_module();

    NUX_ASSERT(c->target);
    nux_u32_t framebuffer = c->target->gpu.framebuffer_slot;
    nux_u32_t width       = c->target->gpu.width;
    nux_u32_t height      = c->target->gpu.height;

    // Update constants
    nux_gpu_constants_buffer_t constants;
    constants.screen_size = nux_v2u(width, height);
    constants.time        = nux_time_elapsed();
    nux_os_buffer_update(nux_userdata(),
                         c->constants_buffer.slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Begin canvas render
    nux_gpu_encoder_t enc;
    nux_gpu_encoder_init(nux_arena_frame(), &enc);
    nux_gpu_bind_framebuffer(&enc, framebuffer);
    nux_gpu_viewport(&enc, nux_v4(0, 0, 1, 1));
    nux_gpu_bind_pipeline(&enc, module->canvas_pipeline.slot);
    nux_gpu_bind_buffer(
        &enc, NUX_GPU_DESC_CANVAS_CONSTANTS, c->constants_buffer.slot);
    nux_gpu_bind_buffer(
        &enc, NUX_GPU_DESC_CANVAS_BATCHES, c->batches_buffer.slot);
    nux_gpu_bind_buffer(&enc, NUX_GPU_DESC_CANVAS_QUADS, c->quads_buffer.slot);
    nux_u32_t clear_color = nux_color_to_hex(
        nux_palette_get_color(module->active_palette, c->clear_color));
    nux_gpu_clear_color(&enc, clear_color);

    // Submit commands
    nux_gpu_encoder_submit(&enc);
    nux_gpu_encoder_submit(&c->encoder);

    // Reset for next loop
    c->batches_buffer_head = 0;
    c->quads_buffer_head   = 0;
}
