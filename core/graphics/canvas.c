#include "internal.h"

#define QUADS_DEFAULT_SIZE   4096 * 8
#define BATCHES_DEFAULT_SIZE 512

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
    NUX_ENSURE(canvas->quads_buffer_head + count < QUADS_DEFAULT_SIZE,
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
    c->quads_buffer.size = sizeof(nux_gpu_canvas_quad_t) * QUADS_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(&c->quads_buffer), return NUX_NULL);

    // Allocate batches buffer
    c->batches_buffer_head = 0;
    c->batches_buffer.type = NUX_GPU_BUFFER_STORAGE;
    c->batches_buffer.size
        = sizeof(nux_gpu_canvas_batch_t) * BATCHES_DEFAULT_SIZE;
    NUX_CHECK(nux_gpu_buffer_init(&c->batches_buffer), return NUX_NULL);

    // Allocate commands
    NUX_CHECK(nux_gpu_encoder_init(nux_arena_core(), &c->encoder),
              return NUX_NULL);

    // Initialize base active batch
    c->active_batch.mode  = 0;
    c->active_batch.first = 0;
    c->active_batch.count = 0;
    c->active_texture     = 0;

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
