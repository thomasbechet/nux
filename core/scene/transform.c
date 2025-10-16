#include "internal.h"

static void
mark_dirty (nux_u32_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_ASSERT(t);
    if (!t->dirty)
    {
        nux_nid_t child = nux_node_child(e);
        while (child)
        {
            mark_dirty(child);
            child = nux_node_sibling(child);
        }
    }
    t->dirty = NUX_TRUE;
}

nux_m4_t
nux_transform_get_matrix (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_m4_identity());

    if (t->dirty)
    {
        t->global_matrix = nux_m4_trs(t->translation, t->rotation, t->scale);
        nux_m4_t parent_matrix = nux_transform_get_matrix(nux_node_parent(e));
        t->global_matrix       = nux_m4_mul(parent_matrix, t->global_matrix);
        t->dirty               = NUX_FALSE;
    }

    return t->global_matrix;
}
nux_status_t
nux_transform_write (nux_serde_writer_t *s, const void *data)
{
    const nux_transform_t *transform = data;
    nux_serde_write_v3(s, "translation", transform->translation);
    nux_serde_write_q4(s, "rotation", transform->rotation);
    nux_serde_write_v3(s, "scale", transform->scale);
    return NUX_SUCCESS;
}
nux_status_t
nux_transform_read (nux_serde_reader_t *s, void *data)
{
    nux_transform_t *transform = data;
    nux_serde_read_v3(s, "translation", &transform->translation);
    nux_serde_read_q4(s, "rotation", &transform->rotation);
    nux_serde_read_v3(s, "scale", &transform->scale);
    transform->dirty = NUX_TRUE;
    return NUX_SUCCESS;
}

void
nux_transform_add (nux_nid_t e)
{
    nux_transform_t *t = nux_component_add(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->translation = NUX_V3_ZEROS;
    t->rotation    = nux_q4_identity();
    t->scale       = NUX_V3_ONES;
    t->dirty       = NUX_TRUE;
}
void
nux_transform_remove (nux_nid_t e)
{
    nux_node_remove(e, NUX_COMPONENT_TRANSFORM);
}
nux_v3_t
nux_transform_get_local_translation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    return t->translation;
}
nux_q4_t
nux_transform_get_local_rotation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_q4_identity());
    return t->rotation;
}
nux_v3_t
nux_transform_get_local_scale (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    return t->scale;
}
nux_v3_t
nux_transform_get_translation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    nux_v3_t translation;
    nux_m4_trs_decompose(
        nux_transform_get_matrix(e), &translation, NUX_NULL, NUX_NULL);
    return translation;
}
nux_q4_t
nux_transform_get_rotation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return nux_q4_identity());
    // TODO: rotation decomposition
    // nux_q4_t rotation;
    // nux_m4_trs_decompose(
    //     nux_transform_get_matrix(e), NUX_NULL, &rotation, NUX_NULL);
    // return rotation;
    return t->rotation;
}
nux_v3_t
nux_transform_get_scale (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ONES);
    nux_v3_t scale;
    nux_m4_trs_decompose(
        nux_transform_get_matrix(e), &scale, NUX_NULL, NUX_NULL);
    return scale;
}
void
nux_transform_set_translation (nux_nid_t e, nux_v3_t position)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->translation = position;
    mark_dirty(e);
}
void
nux_transform_set_rotation (nux_nid_t e, nux_q4_t rotation)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->rotation = rotation;
    mark_dirty(e);
}
void
nux_transform_set_rotation_euler (nux_nid_t e, nux_v3_t euler)
{
    nux_transform_set_rotation(e, nux_q4_euler(euler));
}
void
nux_transform_set_scale (nux_nid_t e, nux_v3_t scale)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->scale = scale;
    mark_dirty(e);
}
void
nux_transform_set_ortho (nux_nid_t e, nux_v3_t a, nux_v3_t b, nux_v3_t c)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    nux_v3_t x     = nux_v3_normalize(nux_v3_sub(c, a));
    nux_v3_t y     = nux_v3_normalize(nux_v3_sub(b, a));
    nux_v3_t z     = nux_v3_normalize(nux_v3_cross(x, y));
    t->translation = a;
    nux_m3_t rot   = nux_m3_axis(x, y, z);
    t->rotation    = nux_q4_from_m3(rot);
    mark_dirty(e);
}
static nux_v3_t
rotate_v3 (nux_nid_t e, nux_v3_t v)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return NUX_V3_ZEROS);
    return nux_m4_mulv3(nux_transform_get_matrix(e), v, 0);
}
nux_v3_t
nux_transform_forward (nux_nid_t e)
{
    return rotate_v3(e, NUX_V3_FORWARD);
}
nux_v3_t
nux_transform_backward (nux_nid_t e)
{
    return rotate_v3(e, NUX_V3_BACKWARD);
}
nux_v3_t
nux_transform_left (nux_nid_t e)
{
    return rotate_v3(e, NUX_V3_LEFT);
}
nux_v3_t
nux_transform_right (nux_nid_t e)
{
    return rotate_v3(e, NUX_V3_RIGHT);
}
nux_v3_t
nux_transform_up (nux_nid_t e)
{
    return rotate_v3(e, NUX_V3_UP);
}
nux_v3_t
nux_transform_down (nux_nid_t e)
{
    return rotate_v3(e, NUX_V3_DOWN);
}
void
nux_transform_rotate (nux_nid_t e, nux_v3_t axis, nux_f32_t angle)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    t->rotation = nux_q4_mul_axis(t->rotation, axis, angle);
    mark_dirty(e);
}
void
nux_transform_rotate_x (nux_nid_t e, nux_f32_t angle)
{
    nux_transform_rotate(e, NUX_V3_RIGHT, angle);
}
void
nux_transform_rotate_y (nux_nid_t e, nux_f32_t angle)
{
    nux_transform_rotate(e, NUX_V3_UP, angle);
}
void
nux_transform_rotate_z (nux_nid_t e, nux_f32_t angle)
{
    nux_transform_rotate(e, NUX_V3_BACKWARD, angle);
}
void
nux_transform_look_at (nux_nid_t e, nux_v3_t center)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(t, return);
    nux_v3_t eye = nux_transform_get_translation(e);

    nux_v3_t zaxis = nux_v3_normalize(nux_v3_sub(center, eye));
    nux_v3_t xaxis = nux_v3_normalize(nux_v3_cross(zaxis, NUX_V3_UP));
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

    mark_dirty(e);
    t->dirty = NUX_FALSE;
}
