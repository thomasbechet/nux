#include "internal.h"

#define QUADS_DEFAULT_SIZE 4096

static nux_status_t
push_quads (nux_ctx_t    *ctx,
            nux_canvas_t *canvas,
            nux_u64_t    *quads,
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
    canvas->quads_buffer_head = 0;
    canvas->quads_buffer.type = NUX_GPU_BUFFER_STORAGE;
    canvas->quads_buffer.size = sizeof(nux_u64_t) * QUADS_DEFAULT_SIZE;
    NUX_CHECKM(nux_gpu_buffer_init(ctx, &canvas->quads_buffer),
               "Failed to create quads buffer",
               return NUX_FAILURE);
    NUX_CHECKM(
        nux_gpu_command_vec_alloc(ctx->core_arena, 4096, &canvas->commands),
        "Failed to allocate canvas commands buffer",
        return NUX_FAILURE);
    return NUX_SUCCESS;
}
void
nux_canvas_begin (nux_ctx_t *ctx, nux_canvas_t *canvas)
{
    canvas->quads_buffer_head = 0;
    nux_gpu_command_vec_clear(&canvas->commands);
}
void
nux_canvas_end (nux_ctx_t *ctx, nux_canvas_t *canvas)
{
    nux_os_gpu_submit(
        ctx->userdata, canvas->commands.data, canvas->commands.size);
}
