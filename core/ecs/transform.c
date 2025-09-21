#include "internal.h"

static nux_m4_t
find_parent_transform (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    if (t)
    {
        return t->global_matrix;
    }

    nux_eid_t parent = t->parent;
    if (parent)
    {
        return find_parent_transform(ctx, parent);
    }

    return nux_m4_identity();
}

nux_b32_t
nux_transform_update_matrix (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_ASSERT(t);

    // Check parent global matrix update
    nux_eid_t parent = t->parent;
    if (parent)
    {
        nux_transform_update_matrix(ctx, parent);
        if (t)
        {
            t->dirty = NUX_TRUE;
        }
    }

    // Update global matrix from parent
    if (t && t->dirty)
    {
        t->global_matrix = nux_m4_trs(
            t->local_translation, t->local_rotation, t->local_scale);
        if (parent)
        {
            nux_m4_t parent_matrix = find_parent_transform(ctx, parent);
            t->global_matrix = nux_m4_mul(parent_matrix, t->global_matrix);
        }
        t->dirty = NUX_FALSE;
        return NUX_TRUE;
    }

    return NUX_FALSE;
}
nux_status_t
nux_transform_write (nux_serde_writer_t *s,
                     const nux_c8_t     *key,
                     const void         *data)
{
    const nux_transform_t *transform = data;
    nux_serde_write_object(s, key);
    nux_serde_write_v3(s, "translation", transform->local_translation);
    nux_serde_write_q4(s, "rotation", transform->local_rotation);
    nux_serde_write_v3(s, "scale", transform->local_scale);
    nux_serde_write_u32(s, "parent", transform->parent);
    nux_serde_write_end(s);
    return NUX_SUCCESS;
}
nux_status_t
nux_transform_read (nux_serde_reader_t *s, const nux_c8_t *key, void *data)
{
    nux_transform_t *transform = data;
    nux_serde_read_object(s, key);
    nux_serde_read_v3(s, "translation", &transform->local_translation);
    nux_serde_read_q4(s, "rotation", &transform->local_rotation);
    nux_serde_read_v3(s, "scale", &transform->local_scale);
    nux_serde_read_u32(s, "parent", &transform->parent);
    transform->dirty = NUX_TRUE;
    nux_serde_read_end(s);
    return NUX_SUCCESS;
}

void
nux_transform_add (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_add(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_translation = NUX_V3_ZEROS;
    t->local_rotation    = nux_q4_identity();
    t->local_scale       = NUX_V3_ONES;
    t->dirty             = NUX_TRUE;
}
void
nux_transform_remove (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_ecs_remove(ctx, e, NUX_COMPONENT_TRANSFORM);
}
void
nux_transform_set_parent (nux_ctx_t *ctx, nux_eid_t e, nux_eid_t parent)
{
    NUX_ENSURE(e != parent, return, "setting node parent to itself");
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->parent = parent;
    t->dirty  = NUX_TRUE;
}
nux_eid_t
nux_transform_get_parent (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_NULL);
    return t->parent;
}
nux_v3_t
nux_transform_get_local_translation (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    return t->local_translation;
}
nux_q4_t
nux_transform_get_local_rotation (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_q4_identity());
    return t->local_rotation;
}
nux_v3_t
nux_transform_get_local_scale (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    return t->local_scale;
}
nux_v3_t
nux_transform_get_translation (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    nux_transform_update_matrix(ctx, e);
    nux_v3_t translation;
    nux_m4_trs_decompose(t->global_matrix, &translation, NUX_NULL, NUX_NULL);
    return translation;
}
nux_q4_t
nux_transform_get_rotation (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_q4_identity());
    nux_transform_update_matrix(ctx, e);
    nux_q4_t rotation;
    nux_m4_trs_decompose(t->global_matrix, NUX_NULL, &rotation, NUX_NULL);
    return rotation;
}
nux_v3_t
nux_transform_get_scale (nux_ctx_t *ctx, nux_eid_t e)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ONES);
    nux_transform_update_matrix(ctx, e);
    nux_v3_t scale;
    nux_m4_trs_decompose(t->global_matrix, &scale, NUX_NULL, NUX_NULL);
    return scale;
}
void
nux_transform_set_translation (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t position)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_translation = position;
    t->dirty             = NUX_TRUE;
}
void
nux_transform_set_rotation (nux_ctx_t *ctx, nux_eid_t e, nux_q4_t rotation)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_rotation = rotation;
    t->dirty          = NUX_TRUE;
}
void
nux_transform_set_rotation_euler (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t euler)
{
    nux_transform_set_rotation(ctx, e, nux_q4_euler(euler));
}
void
nux_transform_set_scale (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t scale)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_scale = scale;
    t->dirty       = NUX_TRUE;
}
void
nux_transform_set_ortho (
    nux_ctx_t *ctx, nux_eid_t e, nux_v3_t a, nux_v3_t b, nux_v3_t c)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    nux_v3_t x           = nux_v3_normalize(nux_v3_sub(c, a));
    nux_v3_t y           = nux_v3_normalize(nux_v3_sub(b, a));
    nux_v3_t z           = nux_v3_normalize(nux_v3_cross(x, y));
    t->local_translation = a;
    nux_m3_t rot         = nux_m3_axis(x, y, z);
    t->local_rotation    = nux_q4_from_m3(rot);
    t->dirty             = NUX_TRUE;
}
static nux_v3_t
rotate_v3 (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t v)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    nux_transform_update_matrix(ctx, e);
    return nux_m4_mulv3(t->global_matrix, v, 0);
}
nux_v3_t
nux_transform_forward (nux_ctx_t *ctx, nux_eid_t e)
{
    return rotate_v3(ctx, e, NUX_V3_FORWARD);
}
nux_v3_t
nux_transform_backward (nux_ctx_t *ctx, nux_eid_t e)
{
    return rotate_v3(ctx, e, NUX_V3_BACKWARD);
}
nux_v3_t
nux_transform_left (nux_ctx_t *ctx, nux_eid_t e)
{
    return rotate_v3(ctx, e, NUX_V3_LEFT);
}
nux_v3_t
nux_transform_right (nux_ctx_t *ctx, nux_eid_t e)
{
    return rotate_v3(ctx, e, NUX_V3_RIGHT);
}
nux_v3_t
nux_transform_up (nux_ctx_t *ctx, nux_eid_t e)
{
    return rotate_v3(ctx, e, NUX_V3_UP);
}
nux_v3_t
nux_transform_down (nux_ctx_t *ctx, nux_eid_t e)
{
    return rotate_v3(ctx, e, NUX_V3_DOWN);
}
void
nux_transform_rotate (nux_ctx_t *ctx,
                      nux_eid_t  e,
                      nux_v3_t   axis,
                      nux_f32_t  angle)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->local_rotation = nux_q4_mul_axis(t->local_rotation, axis, angle);
    t->dirty          = NUX_TRUE;
}
void
nux_transform_rotate_x (nux_ctx_t *ctx, nux_eid_t e, nux_f32_t angle)
{
    nux_transform_rotate(ctx, e, NUX_V3_RIGHT, angle);
}
void
nux_transform_rotate_y (nux_ctx_t *ctx, nux_eid_t e, nux_f32_t angle)
{
    nux_transform_rotate(ctx, e, NUX_V3_UP, angle);
}
void
nux_transform_rotate_z (nux_ctx_t *ctx, nux_eid_t e, nux_f32_t angle)
{
    nux_transform_rotate(ctx, e, NUX_V3_BACKWARD, angle);
}
void
nux_transform_look_at (nux_ctx_t *ctx, nux_eid_t e, nux_v3_t center)
{
    nux_transform_t *t = nux_ecs_get(ctx, e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    nux_transform_update_matrix(ctx, e);
    nux_v3_t eye = nux_transform_get_translation(ctx, e);

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
