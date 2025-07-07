#include "internal.h"

nux_status_t
nux_gpu_buffer_init (nux_ctx_t *ctx, nux_gpu_buffer_t *buffer)
{
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_buffer_slots);
    NUX_CHECKM(slot, "Out of gpu buffer", return NUX_FAILURE);
    buffer->slot = *slot;
    NUX_CHECK(nux_os_create_buffer(
                  ctx->userdata, buffer->slot, buffer->type, buffer->size),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_gpu_texture_init (nux_ctx_t *ctx, nux_texture_t *texture)
{
    nux_gpu_texture_info_t info = {
        .type   = texture->type,
        .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
        .width  = texture->width,
        .height = texture->height,
    };
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_texture_slots);
    NUX_CHECKM(slot, "Out of gpu textures", return NUX_FAILURE);
    texture->slot = *slot;
    NUX_CHECKM(nux_os_create_texture(ctx->userdata, texture->slot, &info),
               "Failed to create texture",
               return NUX_FAILURE);
    // Create framebuffer
    if (texture->type == NUX_TEXTURE_RENDER_TARGET)
    {
        // Create framebuffer
        slot = nux_u32_vec_pop(&ctx->free_framebuffer_slots);
        NUX_CHECKM(slot, "Out of gpu framebuffer slots", return NUX_FAILURE);
        texture->framebuffer_slot = *slot;
        NUX_CHECKM(nux_os_create_framebuffer(
                       ctx->userdata, texture->framebuffer_slot, texture->slot),
                   "Failed to create framebuffer",
                   return NUX_FAILURE);
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_gpu_pipeline_init (nux_ctx_t *ctx, nux_gpu_pipeline_t *pipeline)
{
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_pipeline_slots);
    NUX_CHECKM(slot, "Out of gpu pipelines", return NUX_FAILURE);
    pipeline->slot               = *slot;
    nux_gpu_pipeline_info_t info = { .type = NUX_GPU_PIPELINE_MAIN };
    NUX_CHECKM(nux_os_create_pipeline(ctx->userdata, pipeline->slot, &info),
               "Failed to create pipeline",
               return NUX_FAILURE);
    return NUX_SUCCESS;
}

void
nux_gpu_draw (nux_gpu_command_vec_t *cmds, nux_u32_t count)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type       = NUX_GPU_COMMAND_DRAW;
    cmd->draw.count = count;
}
void
nux_gpu_push_u32 (nux_gpu_command_vec_t *cmds, nux_u32_t index, nux_u32_t value)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type                = NUX_GPU_COMMAND_PUSH_CONSTANT;
    cmd->push_constant.index = index;
    cmd->push_constant.u32   = value;
}
void
nux_gpu_push_f32 (nux_gpu_command_vec_t *cmds, nux_u32_t index, nux_f32_t value)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type                = NUX_GPU_COMMAND_PUSH_CONSTANT;
    cmd->push_constant.index = index;
    cmd->push_constant.f32   = value;
}
void
nux_gpu_bind_pipeline (nux_gpu_command_vec_t *cmds, nux_u32_t slot)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type               = NUX_GPU_COMMAND_BIND_PIPELINE;
    cmd->bind_pipeline.slot = slot;
}
void
nux_gpu_bind_texture (nux_gpu_command_vec_t *cmds,
                      nux_u32_t              index,
                      nux_u32_t              slot)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type               = NUX_GPU_COMMAND_BIND_TEXTURE;
    cmd->bind_texture.slot  = slot;
    cmd->bind_texture.index = index;
}
void
nux_gpu_bind_buffer (nux_gpu_command_vec_t *cmds,
                     nux_u32_t              index,
                     nux_u32_t              slot)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type              = NUX_GPU_COMMAND_BIND_BUFFER;
    cmd->bind_buffer.slot  = slot;
    cmd->bind_buffer.index = index;
}
