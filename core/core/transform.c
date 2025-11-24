#include "internal.h"

static void
mark_dirty (nux_transform_t *transform)
{
    nux_assert(node);
    if (!transform->dirty)
    {
        nux_node_t *child = nux_node_child(node);
        while (child)
        {
            mark_dirty(child);
            child = nux_node_sibling(child);
        }
    }
    node->transform.dirty = true;
}

nux_m4_t
nux_transform_matrix (nux_transform_t *transform)
{
    nux_check(transform, return nux_m4_identity());

    if (transform->dirty)
    {
        transform->global_matrix = nux_m4_trs(
            transform->translation, transform->rotation, transform->scale);
        nux_m4_t parent_matrix = nux_transform_matrix(nux_node_parent(node));
        transform->global_matrix
            = nux_m4_mul(parent_matrix, node->transform.global_matrix);
        node->transform.dirty = false;
    }

    return node->transform.global_matrix;
}

void
nux_transform_init (nux_node_t *node)
{
    nux_transform_t *t = &node->transform;
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
nux_transform_local_translation (nux_transform_t *transform)
{
    return transform->translation;
}
nux_q4_t
nux_transform_local_rotation (nux_transform_t *transform)
{
    return transform->rotation;
}
nux_v3_t
nux_transform_local_scale (nux_transform_t *transform)
{
    return transform->scale;
}
nux_v3_t
nux_transform_translation (nux_transform_t *transform)
{
    nux_v3_t translation;
    nux_m4_trs_decompose(
        nux_transform_matrix(transform), &translation, nullptr, nullptr);
    return translation;
}
nux_q4_t
nux_transform_rotation (nux_transform_t *transform)
{
    // TODO: rotation decomposition
    // nux_q4_t rotation;
    // nux_m4_trs_decompose(
    //     nux_transform_get_matrix(e), nullptr, &rotation, nullptr);
    // return rotation;
    return transform->rotation;
}
nux_v3_t
nux_transform_scale (nux_transform_t *transform)
{
    nux_v3_t scale;
    nux_m4_trs_decompose(
        nux_transform_matrix(transform), &scale, nullptr, nullptr);
    return scale;
}
void
nux_transform_set_translation (nux_transform_t *transform, nux_v3_t position)
{
    transform->translation = position;
    mark_dirty(transform);
}
void
nux_transform_set_rotation (nux_transform_t *transform, nux_q4_t rotation)
{
    transform->rotation = rotation;
    mark_dirty(transform);
}
void
nux_transform_set_rotation_euler (nux_transform_t *transform, nux_v3_t euler)
{
    nux_transform_set_rotation(transform, nux_q4_euler(euler));
}
void
nux_transform_set_scale (nux_transform_t *transform, nux_v3_t scale)
{
    transform->scale = scale;
    mark_dirty(transform);
}
void
nux_transform_set_ortho (nux_transform_t *transform,
                         nux_v3_t         a,
                         nux_v3_t         b,
                         nux_v3_t         c)
{
    nux_v3_t x             = nux_v3_normalize(nux_v3_sub(c, a));
    nux_v3_t y             = nux_v3_normalize(nux_v3_sub(b, a));
    nux_v3_t z             = nux_v3_normalize(nux_v3_cross(x, y));
    transform->translation = a;
    nux_m3_t rot           = nux_m3_axis(x, y, z);
    transform->rotation    = nux_q4_from_m3(rot);
    mark_dirty(transform);
}
static nux_v3_t
rotate_v3 (nux_transform_t *transform, nux_v3_t v)
{
    return nux_m4_mulv3(nux_transform_matrix(transform), v, 0);
}
nux_v3_t
nux_transform_forward (nux_transform_t *transform)
{
    return rotate_v3(transform, nux_v3_forward());
}
nux_v3_t
nux_transform_backward (nux_transform_t *transform)
{
    return rotate_v3(transform, nux_v3_backward());
}
nux_v3_t
nux_transform_left (nux_transform_t *transform)
{
    return rotate_v3(transform, nux_v3_left());
}
nux_v3_t
nux_transform_right (nux_transform_t *transform)
{
    return rotate_v3(transform, nux_v3_right());
}
nux_v3_t
nux_transform_up (nux_transform_t *transform)
{
    return rotate_v3(transform, nux_v3_up());
}
nux_v3_t
nux_transform_down (nux_transform_t *transform)
{
    return rotate_v3(transform, nux_v3_down());
}
void
nux_transform_rotate (nux_transform_t *transform,
                      nux_v3_t         axis,
                      nux_f32_t        angle)
{
    transform->rotation = nux_q4_mul_axis(transform->rotation, axis, angle);
    mark_dirty(transform);
}
void
nux_transform_rotate_x (nux_transform_t *transform, nux_f32_t angle)
{
    nux_transform_rotate(transform, nux_v3_right(), angle);
}
void
nux_transform_rotate_y (nux_transform_t *transform, nux_f32_t angle)
{
    nux_transform_rotate(transform, nux_v3_up(), angle);
}
void
nux_transform_rotate_z (nux_transform_t *transform, nux_f32_t angle)
{
    nux_transform_rotate(transform, nux_v3_backward(), angle);
}
void
nux_transform_look_at (nux_transform_t *transform, nux_v3_t center)
{
    nux_v3_t eye = nux_transform_translation(transform);

    nux_v3_t zaxis = nux_v3_normalize(nux_v3_sub(center, eye));
    nux_v3_t xaxis = nux_v3_normalize(nux_v3_cross(zaxis, nux_v3_up()));
    nux_v3_t yaxis = nux_v3_cross(xaxis, zaxis);

    transform->global_matrix.x1 = xaxis.x;
    transform->global_matrix.x2 = xaxis.y;
    transform->global_matrix.x3 = xaxis.z;

    transform->global_matrix.y1 = yaxis.x;
    transform->global_matrix.y2 = yaxis.y;
    transform->global_matrix.y3 = yaxis.z;

    transform->global_matrix.z1 = -zaxis.x;
    transform->global_matrix.z2 = -zaxis.y;
    transform->global_matrix.z3 = -zaxis.z;

    mark_dirty(transform);
    transform->dirty = false;
}
