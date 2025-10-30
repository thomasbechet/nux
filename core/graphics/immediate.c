#include "internal.h"

static void
draw_line (nux_v3_t a, nux_v3_t b)
{
    nux_graphics_module_t *gfx = nux_graphics();

    const nux_v3_t  positions[]  = { a, b };
    const nux_u32_t vertex_count = NUX_ARRAY_SIZE(positions);
    const nux_u32_t stride       = 3;
    nux_f32_t       data[stride * vertex_count];
    nux_v4_t        c = nux_palette_get_color(gfx->active_palette,
                                       gfx->immediate_state->immediate_color);
    for (nux_u32_t i = 0; i < vertex_count; ++i)
    {
        data[i * stride + 0] = positions[i].x;
        data[i * stride + 1] = positions[i].y;
        data[i * stride + 2] = positions[i].z;
    }
    nux_u32_t offset;
    NUX_CHECK(
        nux_graphics_push_frame_vertices(vertex_count * stride, data, &offset),
        return);

    nux_graphics_command_t *cmd
        = nux_graphics_command_vec_push(&gfx->immediate_commands);
    NUX_CHECK(cmd, return);
    cmd->vertex_offset     = offset;
    cmd->vertex_count      = vertex_count;
    cmd->vertex_attributes = NUX_VERTEX_POSITION;
    cmd->transform_offset  = gfx->immediate_state->immediate_transform;
    cmd->color             = c;
    cmd->layer             = gfx->immediate_state->immediate_layer;
}

void
nux_graphics_begin_state (void)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_immediate_state_t *state
        = nux_immediate_state_vec_push(&gfx->immediate_states);
    NUX_CHECK(gfx->immediate_state, return);
    gfx->immediate_state = state;
    nux_graphics_reset_state();
}
void
nux_graphics_end_state (void)
{
    nux_graphics_module_t *gfx = nux_graphics();
    NUX_ENSURE(
        gfx->immediate_states.size > 1, return, "invalid graphics end state");
    gfx->immediate_state
        = gfx->immediate_states.data + gfx->immediate_states.size;
    nux_immediate_state_vec_pop(&gfx->immediate_states);
}
void
nux_graphics_reset_state (void)
{
    nux_graphics_set_transform_identity();
    nux_graphics_set_color(NUX_COLOR_WHITE);
    nux_graphics_set_layer(NUX_LAYER_DEFAULT);
}
void
nux_graphics_draw_line (nux_v3_t a, nux_v3_t b)
{
    nux_graphics_module_t *gfx = nux_graphics();
    draw_line(a, b);
}
void
nux_graphics_draw_dir (nux_v3_t origin, nux_v3_t dir, nux_f32_t length)
{
    nux_v3_t a = origin;
    nux_v3_t b = nux_v3_add(origin, nux_v3_muls(dir, length));
    nux_graphics_draw_line(a, b);
}
void
nux_graphics_set_layer (nux_u32_t layer)
{
    nux_graphics_module_t *gfx            = nux_graphics();
    gfx->immediate_state->immediate_layer = layer;
}
void
nux_graphics_set_color (nux_u32_t color)
{
    nux_graphics_module_t *gfx            = nux_graphics();
    gfx->immediate_state->immediate_color = color;
}
void
nux_graphics_set_transform (nux_m4_t transform)
{
    nux_graphics_module_t *gfx = nux_graphics();
    nux_u32_t              transform_index;
    NUX_CHECK(
        nux_graphics_push_frame_transforms(1, &transform, &transform_index),
        return);
    gfx->immediate_state->immediate_transform = transform_index;
}
void
nux_graphics_set_transform_identity (void)
{
    nux_graphics_module_t *gfx                = nux_graphics();
    gfx->immediate_state->immediate_transform = gfx->identity_transform_offset;
}
