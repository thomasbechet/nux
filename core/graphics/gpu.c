#include "internal.h"

nux_status_t
nux_gpu_buffer_init (nux_gpu_buffer_t *buffer)
{
    nux_graphics_module_t *gfx  = nux_graphics();
    nux_u32_t             *slot = nux_vec_pop(&gfx->free_buffer_slots);
    nux_ensure(slot, return NUX_FAILURE, "out of gpu buffer");
    buffer->slot = *slot;
    nux_check(nux_os_buffer_create(buffer->slot, buffer->type, buffer->size),
              return NUX_FAILURE);
    return NUX_SUCCESS;
}
void
nux_gpu_buffer_free (nux_gpu_buffer_t *buffer)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_os_buffer_delete(buffer->slot);
    nux_vec_pushv(&gfx->free_buffer_slots, buffer->slot);
}
nux_status_t
nux_gpu_texture_init (nux_gpu_texture_t *texture)
{
    nux_graphics_module_t *gfx  = nux_graphics();
    nux_gpu_texture_info_t info = {
        .type   = texture->type,
        .filter = NUX_GPU_TEXTURE_FILTER_NEAREST,
        .width  = texture->width,
        .height = texture->height,
    };
    nux_u32_t *slot = nux_vec_pop(&gfx->free_texture_slots);
    nux_ensure(slot, return NUX_FAILURE, "out of gpu textures");
    texture->slot = *slot;
    nux_ensure(nux_os_texture_create(texture->slot, &info),
               return NUX_FAILURE,
               "failed to create texture");
    // Create framebuffer
    if (texture->type == NUX_TEXTURE_RENDER_TARGET)
    {
        // Create framebuffer
        slot = nux_vec_pop(&gfx->free_framebuffer_slots);
        nux_ensure(slot, return NUX_FAILURE, "out of gpu framebuffer slots");
        texture->framebuffer_slot = *slot;
        nux_ensure(
            nux_os_framebuffer_create(texture->framebuffer_slot, texture->slot),
            return NUX_FAILURE,
            "failed to create framebuffer");
    }
    return NUX_SUCCESS;
}
void
nux_gpu_texture_free (nux_gpu_texture_t *texture)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_os_texture_delete(texture->slot);
    nux_vec_pushv(&gfx->free_texture_slots, texture->slot);
}
nux_status_t
nux_gpu_pipeline_init (nux_gpu_pipeline_t *pipeline)
{
    nux_graphics_module_t *gfx  = nux_graphics();
    nux_u32_t             *slot = nux_vec_pop(&gfx->free_pipeline_slots);
    nux_ensure(slot, return NUX_FAILURE, "out of gpu pipelines");
    pipeline->slot = *slot;
    nux_ensure(nux_os_pipeline_create(pipeline->slot, &pipeline->info),
               return NUX_FAILURE,
               "failed to create pipeline");
    return NUX_SUCCESS;
}
void
nux_gpu_pipeline_free (nux_gpu_pipeline_t *pipeline)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_os_pipeline_delete(pipeline->slot);
    nux_vec_pushv(&gfx->free_pipeline_slots, pipeline->slot);
}

static nux_gpu_command_t *
push_cmd (nux_gpu_encoder_t *enc)
{
    return nux_vec_push(&enc->cmds);
}
void
nux_gpu_encoder_init (nux_arena_t *arena, nux_gpu_encoder_t *enc)
{
    nux_vec_init(&enc->cmds, arena);
}
void
nux_gpu_encoder_submit (nux_gpu_encoder_t *enc)
{
    nux_os_gpu_submit(enc->cmds.data, enc->cmds.size);
    nux_vec_clear(&enc->cmds);
}
void
nux_gpu_bind_framebuffer (nux_gpu_encoder_t *enc, nux_u32_t slot)
{
    nux_gpu_command_t *cmd     = push_cmd(enc);
    cmd->type                  = NUX_GPU_COMMAND_BIND_FRAMEBUFFER;
    cmd->bind_framebuffer.slot = slot;
}
void
nux_gpu_bind_pipeline (nux_gpu_encoder_t *enc, nux_u32_t slot)
{
    nux_gpu_command_t *cmd  = push_cmd(enc);
    cmd->type               = NUX_GPU_COMMAND_BIND_PIPELINE;
    cmd->bind_pipeline.slot = slot;
}
void
nux_gpu_bind_texture (nux_gpu_encoder_t *enc, nux_u32_t desc, nux_u32_t slot)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_BIND_TEXTURE;
    cmd->bind_texture.slot = slot;
    cmd->bind_texture.desc = desc;
}
void
nux_gpu_bind_buffer (nux_gpu_encoder_t *enc, nux_u32_t desc, nux_u32_t slot)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_BIND_BUFFER;
    cmd->bind_buffer.slot  = slot;
    cmd->bind_buffer.desc  = desc;
}
void
nux_gpu_push_u32 (nux_gpu_encoder_t *enc, nux_u32_t desc, nux_u32_t value)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_PUSH_U32;
    cmd->push_u32.desc     = desc;
    cmd->push_u32.value    = value;
}
void
nux_gpu_push_f32 (nux_gpu_encoder_t *enc, nux_u32_t desc, nux_f32_t value)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_PUSH_F32;
    cmd->push_f32.desc     = desc;
    cmd->push_f32.value    = value;
}
void
nux_gpu_push_v2 (nux_gpu_encoder_t *enc, nux_u32_t desc, nux_v2_t value)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_PUSH_F32;
    cmd->push_f32.desc     = desc;
}
void
nux_gpu_draw (nux_gpu_encoder_t *enc, nux_u32_t count)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_DRAW;
    cmd->draw.count        = count;
}
void
nux_gpu_draw_full_quad (nux_gpu_encoder_t *enc)
{
    nux_gpu_draw(enc, 3); // use extented triangle to cover full screen
}
void
nux_gpu_clear_color (nux_gpu_encoder_t *enc, nux_u32_t color)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_CLEAR_COLOR;
    cmd->clear_color.color = color;
}
void
nux_gpu_clear_depth (nux_gpu_encoder_t *enc)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_CLEAR_DEPTH;
}
void
nux_gpu_viewport (nux_gpu_encoder_t *enc, nux_v4_t viewport)
{
    nux_gpu_command_t *cmd = push_cmd(enc);
    cmd->type              = NUX_GPU_COMMAND_VIEWPORT;
    cmd->viewport.extent   = viewport;
}
