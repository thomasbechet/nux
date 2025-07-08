#include "internal.h"

#define QUADS_DEFAULT_SIZE 4096

typedef struct
{
    nux_u32_t pos;
    nux_u32_t tex;
    nux_u32_t size;
} gpu_quad_t;

static gpu_quad_t
make_quad (nux_u16_t x,
           nux_u16_t y,
           nux_u16_t tx,
           nux_u16_t ty,
           nux_u16_t sx,
           nux_u16_t sy)
{
    gpu_quad_t q;
    q.pos  = ((nux_u32_t)y << 16) | x;
    q.tex  = ((nux_u32_t)ty << 16) | tx;
    q.size = ((nux_u32_t)sy << 16) | sx;
    // blit->depth
    //     = (pass->depth - NUGL__MIN_DEPTH) / (NUGL__MAX_DEPTH -
    //     NUGL__MIN_DEPTH);
    return q;
}
static nux_status_t
push_quads (nux_ctx_t    *ctx,
            nux_canvas_t *canvas,
            gpu_quad_t   *quads,
            nux_u32_t     count,
            nux_u32_t    *index)
{
    NUX_CHECKM(canvas->quads_buffer_head + count < QUADS_DEFAULT_SIZE,
               "Out of quads",
               return NUX_FAILURE);
    NUX_CHECKM(nux_os_update_buffer(ctx->userdata,
                                    canvas->quads_buffer.slot,
                                    canvas->quads_buffer_head * sizeof(*quads),
                                    count * sizeof(*quads),
                                    quads),
               "Failed to update quads buffer",
               return NUX_FAILURE);
    *index = canvas->quads_buffer_head;
    canvas->quads_buffer_head += count;
    return NUX_SUCCESS;
}
static void
render_quads (nux_ctx_t             *ctx,
              nux_gpu_command_vec_t *cmds,
              nux_u32_t              first,
              nux_u32_t              count)
{
    nux_gpu_push_u32(cmds, NUX_GPU_INDEX_CANVAS_FIRST_QUAD, first);
    nux_gpu_draw(cmds, 6 * count);
}

nux_status_t
nux_canvas_init (nux_ctx_t *ctx, nux_canvas_t *canvas)
{
    // Allocate constants buffer
    canvas->constants_buffer.type = NUX_GPU_BUFFER_UNIFORM;
    canvas->constants_buffer.size = sizeof(nux_gpu_constants_buffer_t);
    NUX_CHECKM(nux_gpu_buffer_init(ctx, &canvas->constants_buffer),
               "Failed to create constants buffer",
               return NUX_NULL);

    // Allocate quads buffer
    canvas->quads_buffer_head = 0;
    canvas->quads_buffer.type = NUX_GPU_BUFFER_STORAGE;
    canvas->quads_buffer.size = sizeof(gpu_quad_t) * QUADS_DEFAULT_SIZE;
    NUX_CHECKM(nux_gpu_buffer_init(ctx, &canvas->quads_buffer),
               "Failed to create quads buffer",
               return NUX_FAILURE);

    // Allocate commands
    NUX_CHECKM(
        nux_gpu_command_vec_alloc(ctx->core_arena, 4096, &canvas->commands),
        "Failed to allocate canvas commands buffer",
        return NUX_FAILURE);

    return NUX_SUCCESS;
}
static void
nux_canvas_draw_text (nux_ctx_t      *ctx,
                      nux_canvas_t   *canvas,
                      nux_font_t     *font,
                      nux_u32_t       x,
                      nux_u32_t       y,
                      const nux_c8_t *text)
{
    nux_gpu_bind_texture(
        &canvas->commands, NUX_GPU_INDEX_CANVAS_TEXTURE, font->texture.slot);
    nux_gpu_push_u32(&canvas->commands,
                     NUX_GPU_INDEX_CANVAS_ATLAS_WIDTH,
                     font->texture.width);
    nux_gpu_push_u32(&canvas->commands,
                     NUX_GPU_INDEX_CANVAS_ATLAS_HEIGHT,
                     font->texture.height);

    nux_u32_t len = nux_strnlen(text, 1024);
    nux_b2i_t extent
        = nux_b2i_xywh(x, y, font->glyph_width, font->glyph_height);
    nux_u32_t       quad_count = 0;
    nux_u32_t       first_quad = 0;
    const nux_c8_t *it         = text;
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
        nux_u32_t  index      = font->char_to_glyph_index[(nux_u8_t)*it];
        gpu_quad_t quad       = make_quad(extent.min.x,
                                    extent.min.y,
                                    index * font->glyph_width,
                                    0,
                                    font->glyph_width,
                                    font->glyph_height);
        nux_u32_t  quad_index = 0;
        NUX_CHECK(push_quads(ctx, canvas, &quad, 1, &index), return);
        if (!first_quad) // Keep track of first quad index
        {
            first_quad = quad_index;
        }

        // Add quad to buffer
        ++quad_count;

        // Move to next character
        extent = nux_b2i_translate(extent, nux_v2i(font->glyph_width - 1, 0));
        ++it;
    }

    // Render quads
    render_quads(ctx, &canvas->commands, first_quad, quad_count);
}

nux_u32_t
nux_canvas_new (nux_ctx_t *ctx)
{
    nux_canvas_t *c = nux_arena_alloc(ctx->active_arena, sizeof(*c));
    NUX_CHECK(c, return NUX_NULL);
    nux_u32_t id
        = nux_object_create(ctx, ctx->active_arena, NUX_TYPE_CANVAS, c);
    NUX_CHECK(id, return NUX_NULL);
    NUX_CHECK(nux_canvas_init(ctx, c), return NUX_NULL);
    return id;
}
void
nux_canvas_clear (nux_ctx_t *ctx, nux_u32_t id)
{
    nux_canvas_t *c = nux_object_get(ctx, NUX_TYPE_CANVAS, id);
    NUX_CHECK(c, return);
    c->quads_buffer_head = 0;
    nux_gpu_command_vec_clear(&c->commands);
}
void
nux_canvas_render (nux_ctx_t *ctx, nux_u32_t id, nux_u32_t target)
{
    nux_canvas_t *c = nux_object_get(ctx, NUX_TYPE_CANVAS, id);
    NUX_CHECK(c, return);

    nux_gpu_command_t     cmds_data[16];
    nux_gpu_command_vec_t cmds;
    nux_gpu_command_vec_init(cmds_data, NUX_ARRAY_SIZE(cmds_data), &cmds);

    nux_u32_t framebuffer = NUX_NULL;
    nux_u32_t width       = 1000;
    nux_u32_t height      = 800;
    if (target)
    {
        nux_texture_t *rt = nux_object_get(ctx, NUX_TYPE_TEXTURE, target);
        if (rt->gpu.type != NUX_TEXTURE_RENDER_TARGET)
        {
            NUX_ERROR("Canvas target is a render target");
            return;
        }
        framebuffer = rt->gpu.framebuffer_slot;
        width       = rt->gpu.width;
        height      = rt->gpu.height;
    }

    // Update constants
    nux_gpu_constants_buffer_t constants;
    constants.screen_size = nux_v2u(width, height);
    constants.canvas_size = nux_v2u(width, height);
    constants.time        = ctx->time;
    nux_os_update_buffer(ctx->userdata,
                         c->constants_buffer.slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Begin canvas render
    nux_gpu_bind_framebuffer(&cmds, framebuffer);
    // nux_gpu_clear(&cmds, 0x770000);
    nux_gpu_bind_pipeline(&cmds, ctx->canvas_pipeline.slot);
    nux_gpu_bind_buffer(
        &cmds, NUX_GPU_INDEX_CANVAS_CONSTANTS, c->constants_buffer.slot);
    nux_gpu_bind_buffer(
        &cmds, NUX_GPU_INDEX_CANVAS_QUADS, c->quads_buffer.slot);

    // Submit commands
    nux_os_gpu_submit(ctx->userdata, cmds.data, cmds.size);
    nux_os_gpu_submit(ctx->userdata, c->commands.data, c->commands.size);
}
void
nux_canvas_text (nux_ctx_t      *ctx,
                 nux_u32_t       id,
                 nux_u32_t       x,
                 nux_u32_t       y,
                 const nux_c8_t *text)
{
    nux_canvas_t *c = nux_object_get(ctx, NUX_TYPE_CANVAS, id);
    NUX_CHECK(c, return);
    nux_canvas_draw_text(ctx, c, &ctx->default_font, x, y, text);
}
