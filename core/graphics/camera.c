#include "internal.h"

nux_status_t
nux_camera_write (nux_serde_writer_t *s, const void *data)
{
    const nux_camera_t *camera = data;
    nux_serde_write_f32(s, "far", camera->far);
    nux_serde_write_f32(s, "near", camera->near);
    nux_serde_write_f32(s, "fov", camera->fov);
    return NUX_SUCCESS;
}
nux_status_t
nux_camera_read (nux_serde_reader_t *s, void *data)
{
    nux_camera_t *camera = data;
    nux_serde_read_f32(s, "far", &camera->far);
    nux_serde_read_f32(s, "near", &camera->near);
    nux_serde_read_f32(s, "fov", &camera->fov);
    return NUX_SUCCESS;
}

void
nux_camera_add (nux_nid_t e)
{
    nux_camera_t *c = nux_component_add(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->fov      = 60;
    c->near     = 0.1;
    c->far      = 200;
    c->viewport = nux_b2i_xywh(0, 0, 100, 100);
}
void
nux_camera_remove (nux_nid_t e)
{
    nux_node_remove(e, NUX_COMPONENT_CAMERA);
}
void
nux_camera_set_fov (nux_nid_t e, nux_f32_t fov)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->fov = fov;
}
void
nux_camera_set_near (nux_nid_t e, nux_f32_t near)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->near = near;
}
void
nux_camera_set_far (nux_nid_t e, nux_f32_t far)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->far = far;
}
nux_v3_t
nux_camera_unproject (nux_nid_t e, nux_v2_t pos)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return NUX_V3_ZEROS);
    nux_transform_t *ct = nux_component_get(e, NUX_COMPONENT_TRANSFORM);
    NUX_CHECK(ct, return NUX_V3_ZEROS);

    nux_v3_t eye    = nux_m4_mulv3(ct->global_matrix, NUX_V3_ZEROS, 1);
    nux_v3_t center = nux_m4_mulv3(ct->global_matrix, NUX_V3_FORWARD, 1);
    nux_v3_t up     = nux_m4_mulv3(ct->global_matrix, NUX_V3_UP, 0);

    nux_m4_t view = nux_m4_lookat(eye, center, up);
    nux_m4_t proj
        = nux_m4_perspective(nux_radian(c->fov),
                          (nux_f32_t)NUX_CANVAS_WIDTH / NUX_CANVAS_HEIGHT,
                          c->near,
                          c->far);
    nux_m4_t vp  = nux_m4_mul(proj, view);
    nux_m4_t inv = nux_m4_inv(vp);

    pos.y = 1 - pos.y; // convert screen to ndc space
    pos   = nux_v2_subs(nux_v2_muls(pos, 2), 1);
    return nux_v3_normalize(nux_m4_mulv3(inv, nux_v3(pos.x, pos.y, 1), 1));
}
