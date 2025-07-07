#include "internal.h"

#define QUADS_DEFAULT_SIZE 4096

typedef struct
{
    nux_u32_t pos;
    nux_u32_t tex;
    nux_u32_t size;
} gpu_quad_t;

static gpu_quad_t
make_quad (nux_u32_t x,
           nux_u32_t y,
           nux_u32_t tx,
           nux_u32_t ty,
           nux_u32_t sx,
           nux_u32_t sy)
{
    gpu_quad_t q;
    q.pos  = (y << 16) | x;
    q.tex  = (ty << 16) | tx;
    q.size = (sy << 16) | sx;
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
void
nux_canvas_begin (nux_ctx_t *ctx, nux_canvas_t *canvas)
{
    // Clear buffers
    canvas->quads_buffer_head = 0;
    nux_gpu_command_vec_clear(&canvas->commands);

    // Begin render
    nux_gpu_bind_pipeline(&canvas->commands, ctx->canvas_pipeline.slot);
    nux_gpu_bind_buffer(&canvas->commands,
                        NUX_GPU_INDEX_CANVAS_CONSTANTS,
                        canvas->constants_buffer.slot);
    nux_gpu_bind_buffer(&canvas->commands,
                        NUX_GPU_INDEX_CANVAS_QUADS,
                        canvas->quads_buffer.slot);

    // TEST
    gpu_quad_t q = make_quad(50, 50, 0, 0, 100, 100);
    nux_u32_t  index;
    NUX_ASSERT(push_quads(ctx, canvas, &q, 1, &index));
    nux_gpu_push_u32(&canvas->commands, NUX_GPU_INDEX_CANVAS_FIRST_QUAD, index);
    nux_gpu_draw(&canvas->commands, 6);
}
void
nux_canvas_end (nux_ctx_t *ctx, nux_canvas_t *canvas)
{
    // Update constants
    nux_gpu_constants_buffer_t constants;
    constants.screen_size = nux_v2u(ctx->stats[NUX_STAT_SCREEN_WIDTH],
                                    ctx->stats[NUX_STAT_SCREEN_HEIGHT]);
    constants.canvas_size = nux_v2u(NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);
    constants.time        = ctx->time;
    nux_os_update_buffer(ctx->userdata,
                         canvas->constants_buffer.slot,
                         0,
                         sizeof(constants),
                         &constants);

    // Submit commands
    nux_os_gpu_submit(
        ctx->userdata, canvas->commands.data, canvas->commands.size);
}
