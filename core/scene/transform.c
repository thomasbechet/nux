#include "internal.h"

static void
mark_dirty (nux_u32_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_assert(t);
    if (!t->dirty)
    {
        nux_nid_t child = nux_node_child(e);
        while (child)
        {
            mark_dirty(child);
            child = nux_node_sibling(child);
        }
    }
    t->dirty = true;
}

nux_m4_t
nux_transform_matrix (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_m4_identity());

    if (t->dirty)
    {
        t->global_matrix = nux_m4_trs(t->translation, t->rotation, t->scale);
        nux_m4_t parent_matrix = nux_transform_matrix(nux_node_parent(e));
        t->global_matrix       = nux_m4_mul(parent_matrix, t->global_matrix);
        t->dirty               = false;
    }

    return t->global_matrix;
}

void
nux_transform_add (nux_nid_t e, void *data)
{
    nux_transform_t *t = data;
    t->translation     = nux_v3_zero();
    t->rotation        = nux_q4_identity();
    t->scale           = nux_v3_one();
    t->dirty           = true;
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
    transform->dirty = true;
    return NUX_SUCCESS;
}

nux_v3_t
nux_transform_local_translation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_v3_zero());
    return t->translation;
}
nux_q4_t
nux_transform_local_rotation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_q4_identity());
    return t->rotation;
}
nux_v3_t
nux_transform_local_scale (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_v3_zero());
    return t->scale;
}
nux_v3_t
nux_transform_translation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_v3_zero());
    nux_v3_t translation;
    nux_m4_trs_decompose(
        nux_transform_matrix(e), &translation, nullptr, nullptr);
    return translation;
}
nux_q4_t
nux_transform_rotation (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_q4_identity());
    // TODO: rotation decomposition
    // nux_q4_t rotation;
    // nux_m4_trs_decompose(
    //     nux_transform_get_matrix(e), nullptr, &rotation, nullptr);
    // return rotation;
    return t->rotation;
}
nux_v3_t
nux_transform_scale (nux_nid_t e)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return nux_v3_one());
    nux_v3_t scale;
    nux_m4_trs_decompose(nux_transform_matrix(e), &scale, nullptr, nullptr);
    return scale;
}
void
nux_transform_set_translation (nux_nid_t e, nux_v3_t position)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return);
    t->translation = position;
    mark_dirty(e);
}
void
nux_transform_set_rotation (nux_nid_t e, nux_q4_t rotation)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return);
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
    nux_check(t, return);
    t->scale = scale;
    mark_dirty(e);
}
void
nux_transform_set_ortho (nux_nid_t e, nux_v3_t a, nux_v3_t b, nux_v3_t c)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return);
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
    nux_check(t, return nux_v3_zero());
    return nux_m4_mulv3(nux_transform_matrix(e), v, 0);
}
nux_v3_t
nux_transform_forward (nux_nid_t e)
{
    return rotate_v3(e, nux_v3_forward());
}
nux_v3_t
nux_transform_backward (nux_nid_t e)
{
    return rotate_v3(e, nux_v3_backward());
}
nux_v3_t
nux_transform_left (nux_nid_t e)
{
    return rotate_v3(e, nux_v3_left());
}
nux_v3_t
nux_transform_right (nux_nid_t e)
{
    return rotate_v3(e, nux_v3_right());
}
nux_v3_t
nux_transform_up (nux_nid_t e)
{
    return rotate_v3(e, nux_v3_up());
}
nux_v3_t
nux_transform_down (nux_nid_t e)
{
    return rotate_v3(e, nux_v3_down());
}
void
nux_transform_rotate (nux_nid_t e, nux_v3_t axis, nux_f32_t angle)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return);
    t->rotation = nux_q4_mul_axis(t->rotation, axis, angle);
    mark_dirty(e);
}
void
nux_transform_rotate_x (nux_nid_t e, nux_f32_t angle)
{
    nux_transform_rotate(e, nux_v3_right(), angle);
}
void
nux_transform_rotate_y (nux_nid_t e, nux_f32_t angle)
{
    nux_transform_rotate(e, nux_v3_up(), angle);
}
void
nux_transform_rotate_z (nux_nid_t e, nux_f32_t angle)
{
    nux_transform_rotate(e, nux_v3_backward(), angle);
}
void
nux_transform_look_at (nux_nid_t e, nux_v3_t center)
{
    nux_transform_t *t = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    nux_check(t, return);
    nux_v3_t eye = nux_transform_translation(e);

    nux_v3_t zaxis = nux_v3_normalize(nux_v3_sub(center, eye));
    nux_v3_t xaxis = nux_v3_normalize(nux_v3_cross(zaxis, nux_v3_up()));
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
    t->dirty = false;
}
