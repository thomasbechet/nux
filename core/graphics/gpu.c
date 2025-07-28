#include "internal.h"

nux_status_t
nux_gpu_buffer_init (nux_ctx_t *ctx, nux_gpu_buffer_t *buffer)
{
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_buffer_slots);
    NUX_ENSURE(slot, return NUX_FAILURE, "out of gpu buffer");
    buffer->slot = *slot;
    NUX_CHECK(nux_os_buffer_create(
                  ctx->userdata, buffer->slot, buffer->type, buffer->size),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
nux_status_t
nux_gpu_texture_init (nux_ctx_t *ctx, nux_gpu_texture_t *texture)
{
    nux_gpu_texture_info_t info = {
        .type   = texture->type,
        .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
        .width  = texture->width,
        .height = texture->height,
    };
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_texture_slots);
    NUX_ENSURE(slot, return NUX_FAILURE, "out of gpu textures");
    texture->slot = *slot;
    NUX_ENSURE(nux_os_texture_create(ctx->userdata, texture->slot, &info),
               return NUX_FAILURE,
               "failed to create texture");
    // Create framebuffer
    if (texture->type == NUX_TEXTURE_RENDER_TARGET)
    {
        // Create framebuffer
        slot = nux_u32_vec_pop(&ctx->free_framebuffer_slots);
        NUX_ENSURE(slot, return NUX_FAILURE, "out of gpu framebuffer slots");
        texture->framebuffer_slot = *slot;
        NUX_ENSURE(nux_os_framebuffer_create(
                       ctx->userdata, texture->framebuffer_slot, texture->slot),
                   return NUX_FAILURE,
                   "failed to create framebuffer");
    }
    return NUX_SUCCESS;
}
nux_status_t
nux_gpu_pipeline_init (nux_ctx_t *ctx, nux_gpu_pipeline_t *pipeline)
{
    nux_u32_t *slot = nux_u32_vec_pop(&ctx->free_pipeline_slots);
    NUX_ENSURE(slot, return NUX_FAILURE, "out of gpu pipelines");
    pipeline->slot = *slot;
    NUX_ENSURE(
        nux_os_pipeline_create(ctx->userdata, pipeline->slot, &pipeline->info),
        return NUX_FAILURE,
        "failed to create pipeline");
    return NUX_SUCCESS;
}

void
nux_gpu_bind_framebuffer (nux_gpu_command_vec_t *cmds, nux_u32_t slot)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type                  = NUX_GPU_COMMAND_BIND_FRAMEBUFFER;
    cmd->bind_framebuffer.slot = slot;
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
                      nux_u32_t              desc,
                      nux_u32_t              slot)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type              = NUX_GPU_COMMAND_BIND_TEXTURE;
    cmd->bind_texture.slot = slot;
    cmd->bind_texture.desc = desc;
}
void
nux_gpu_bind_buffer (nux_gpu_command_vec_t *cmds,
                     nux_u32_t              desc,
                     nux_u32_t              slot)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type             = NUX_GPU_COMMAND_BIND_BUFFER;
    cmd->bind_buffer.slot = slot;
    cmd->bind_buffer.desc = desc;
}
void
nux_gpu_push_u32 (nux_gpu_command_vec_t *cmds, nux_u32_t desc, nux_u32_t value)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type           = NUX_GPU_COMMAND_PUSH_U32;
    cmd->push_u32.desc  = desc;
    cmd->push_u32.value = value;
}
void
nux_gpu_push_f32 (nux_gpu_command_vec_t *cmds, nux_u32_t desc, nux_f32_t value)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type           = NUX_GPU_COMMAND_PUSH_F32;
    cmd->push_f32.desc  = desc;
    cmd->push_f32.value = value;
}
void
nux_gpu_push_v2 (nux_gpu_command_vec_t *cmds, nux_u32_t desc, nux_v2_t value)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type          = NUX_GPU_COMMAND_PUSH_F32;
    cmd->push_f32.desc = desc;
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
nux_gpu_clear (nux_gpu_command_vec_t *cmds, nux_u32_t color)
{
    nux_gpu_command_t *cmd = nux_gpu_command_vec_push(cmds);
    NUX_ASSERT(cmd);
    cmd->type        = NUX_GPU_COMMAND_CLEAR;
    cmd->clear.color = color;
}
