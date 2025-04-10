#include "internal.h"

static nux_command_t *
push_command (nux_env_t env)
{
    NU_ASSERT(env->inst->cmds);
    if (env->inst->cmds_size >= env->inst->cmds_capa)
    {
        nux_set_error(env, NUX_ERROR_OUT_OF_COMMANDS);
        return NU_NULL;
    }
    nux_command_t *cmd = env->inst->cmds + env->inst->cmds_size;
    ++env->inst->cmds_size;
    return cmd;
}

nux_command_t *
nux_instance_get_commands (nux_instance_t inst, nux_u32_t *count)
{
    *count = inst->cmds_size;
    return inst->cmds;
}

nux_u32_t
nux_texture_memsize (nux_u32_t size)
{
    return size * size * 4;
}
nux_u32_t
nux_vertex_memsize (nux_vertex_attribute_t attributes, nux_u32_t count)
{
    nu_u32_t size = 0;
    if (attributes & NUX_VERTEX_POSITION)
    {
        size += NU_V3_SIZE * count;
    }
    if (attributes & NUX_VERTEX_UV)
    {
        size += NU_V2_SIZE * count;
    }
    if (attributes & NUX_VERTEX_COLOR)
    {
        size += NU_V3_SIZE * count;
    }
    return size * sizeof(nu_f32_t);
}
nux_u32_t
nux_vertex_offset (nux_vertex_attribute_t attributes,
                   nux_vertex_attribute_t attribute,
                   nux_u32_t              count)
{
    NU_ASSERT(attribute & attributes);
    nu_u32_t offset = 0;
    if (attributes & NUX_VERTEX_POSITION)
    {
        if (attribute == NUX_VERTEX_POSITION)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    if (attributes & NUX_VERTEX_UV)
    {
        if (attribute == NUX_VERTEX_UV)
        {
            return offset;
        }
        offset += NU_V2_SIZE * count;
    }
    if (attributes & NUX_VERTEX_COLOR)
    {
        if (attribute == NUX_VERTEX_COLOR)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    return offset;
}

void
nux_push_scissor (
    nux_env_t env, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    nux_command_t *cmd = push_command(env);
    NU_CHECK(cmd, return);
    cmd->type       = NUX_COMMAND_PUSH_SCISSOR;
    cmd->scissor[0] = x;
    cmd->scissor[1] = y;
    cmd->scissor[2] = w;
    cmd->scissor[3] = h;
}
void
nux_push_viewport (
    nux_env_t env, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    nux_command_t *cmd = push_command(env);
    NU_CHECK(cmd, return);
    cmd->type        = NUX_COMMAND_PUSH_VIEWPORT;
    cmd->viewport[0] = x;
    cmd->viewport[1] = y;
    cmd->viewport[2] = w;
    cmd->viewport[3] = h;
}
void
nux_push_cursor (nux_env_t env, nux_u32_t x, nux_u32_t y)
{
    nux_command_t *cmd = push_command(env);
    NU_CHECK(cmd, return);
    cmd->type      = NUX_COMMAND_PUSH_CURSOR;
    cmd->cursor[0] = x;
    cmd->cursor[1] = y;
}
void
nux_push_color (nux_env_t env, nux_u32_t color)
{
    nux_command_t *cmd = push_command(env);
    NU_CHECK(cmd, return);
    cmd->type  = NUX_COMMAND_PUSH_COLOR;
    cmd->color = color;
}
void
nux_clear (nux_env_t env, nux_u32_t color)
{
    nux_command_t *cmd = push_command(env);
    NU_CHECK(cmd, return);
    cmd->type  = NUX_COMMAND_CLEAR;
    cmd->clear = color;
}
void
nux_draw_text (nux_env_t env, const nux_c8_t *text)
{
}
void
nux_print (nux_env_t env, const nux_c8_t *text)
{
}
void
nux_blit (nux_env_t env,
          nux_id_t  texture,
          nux_u32_t x,
          nux_u32_t y,
          nux_u32_t w,
          nux_u32_t h)
{
}
void
nux_draw_sprite (nux_env_t env, nux_id_t spritesheet, nux_u32_t sprite)
{
}
void
nux_draw_scene (nux_env_t env, nux_id_t scene, nux_u32_t camera)
{
    nux_command_t *cmd = push_command(env);
    NU_CHECK(cmd, return);
    cmd->type              = NUX_COMMAND_DRAW_SCENE;
    cmd->draw_scene.camera = camera;
}
