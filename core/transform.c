#include "internal.h"

nux_b32_t
nux_transform_update_matrix (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_ASSERT(t);

    // Check parent global matrix update
    if (t->parent)
    {
        nux_transform_update_matrix(ctx, t->parent);
        t->dirty = NUX_TRUE;
    }

    // Update global matrix from parent
    if (t->dirty)
    {
        t->global_matrix = nux_m4_trs(
            t->local_translation, t->local_rotation, t->local_scale);
        if (t->parent)
        {
            nux_transform_t *parent_t = nux_scene_get_component(
                ctx, t->parent, NUX_COMPONENT_TRANSFORM);
            NUX_ASSERT(parent_t);
            t->global_matrix
                = nux_m4_mul(parent_t->global_matrix, t->global_matrix);
        }
        t->dirty = NUX_FALSE;
        return NUX_TRUE;
    }

    return NUX_FALSE;
}

void
nux_transform_add (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_add_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_translation = NUX_V3_ZEROES;
    t->local_rotation    = nux_q4_identity();
    t->local_scale       = NUX_V3_ONES;
    t->dirty             = NUX_TRUE;
    t->parent            = NUX_NULL;
}
void
nux_transform_remove (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_scene_remove_component(ctx, node, NUX_COMPONENT_TRANSFORM);
}
void
nux_transform_set_parent (nux_ctx_t *ctx, nux_u32_t node, nux_u32_t parent)
{
    NUX_CHECKM(node != parent, "Setting transform parent to itself", return);
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->parent = parent;
    t->dirty  = NUX_TRUE;
}
nux_u32_t
nux_transform_get_parent (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_NULL);
    return t->parent;
}
nux_v3_t
nux_transform_get_local_translation (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    return t->local_translation;
}
nux_v3_t
nux_transform_get_local_scale (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    return t->local_scale;
}
nux_v3_t
nux_transform_get_translation (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    nux_transform_update_matrix(ctx, node);
    nux_v3_t translation;
    nux_m4_trs_decompose(t->global_matrix, &translation, NUX_NULL, NUX_NULL);
    return translation;
}
nux_v3_t
nux_transform_get_scale (nux_ctx_t *ctx, nux_u32_t node)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    nux_transform_update_matrix(ctx, node);
    return t->local_scale;
}
static nux_v3_t
rotate_v3 (nux_ctx_t *ctx, nux_u32_t node, nux_v3_t v)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROES);
    nux_transform_update_matrix(ctx, node);
    return nux_m4_mulv3(t->global_matrix, v, 0);
}
nux_v3_t
nux_transform_forward (nux_ctx_t *ctx, nux_u32_t node)
{
    return rotate_v3(ctx, node, NUX_V3_FORWARD);
}
nux_v3_t
nux_transform_backward (nux_ctx_t *ctx, nux_u32_t node)
{
    return rotate_v3(ctx, node, NUX_V3_BACKWARD);
}
nux_v3_t
nux_transform_left (nux_ctx_t *ctx, nux_u32_t node)
{
    return rotate_v3(ctx, node, NUX_V3_LEFT);
}
nux_v3_t
nux_transform_right (nux_ctx_t *ctx, nux_u32_t node)
{
    return rotate_v3(ctx, node, NUX_V3_RIGHT);
}
nux_v3_t
nux_transform_up (nux_ctx_t *ctx, nux_u32_t node)
{
    return rotate_v3(ctx, node, NUX_V3_UP);
}
nux_v3_t
nux_transform_down (nux_ctx_t *ctx, nux_u32_t node)
{
    return rotate_v3(ctx, node, NUX_V3_DOWN);
}
void
nux_transform_set_translation (nux_ctx_t *ctx,
                               nux_u32_t  node,
                               nux_v3_t   position)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_translation = position;
    t->dirty             = NUX_TRUE;
}
void
nux_transform_set_scale (nux_ctx_t *ctx, nux_u32_t node, nux_v3_t scale)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_scale = scale;
    t->dirty       = NUX_TRUE;
}
void
nux_transform_rotate_y (nux_ctx_t *ctx, nux_u32_t node, nux_f32_t angle)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_rotation
        = nux_q4_mul_axis(t->local_rotation, NUX_V3_UP, nux_radian(angle));
    t->dirty = NUX_TRUE;
}
void
nux_transform_look_at (nux_ctx_t *ctx, nux_u32_t node, nux_v3_t center)
{
    nux_transform_t *t
        = nux_scene_get_component(ctx, node, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    nux_transform_update_matrix(ctx, node);
    nux_v3_t eye = nux_transform_get_translation(ctx, node);

    nux_v3_t zaxis = nux_v3_normalize(nux_v3_sub(center, eye));
    nux_v3_t xaxis = nux_v3_normalize(nux_v3_cross(NUX_V3_UP, zaxis));
    nux_v3_t yaxis = nux_v3_cross(xaxis, zaxis);

    t->global_matrix.x1 = xaxis.x;
    t->global_matrix.x2 = xaxis.y;
    t->global_matrix.x3 = xaxis.z;

    t->global_matrix.y1 = yaxis.x;
    t->global_matrix.y2 = yaxis.y;
    t->global_matrix.y3 = yaxis.z;

    t->global_matrix.z1 = -zaxis.x;
    t->global_matrix.z2 = -zaxis.y;
    t->global_matrix.z3 = -zaxis.z;

    t->dirty = NUX_FALSE;
}
