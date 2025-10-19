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
    c->fov         = 60;
    c->near        = 0.1;
    c->far         = 500;
    c->aspect      = 1;
    c->ortho_size  = nux_v2s(1);
    c->ortho       = NUX_FALSE;
    c->render_mask = NUX_LAYER_DEFAULT;
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
void
nux_camera_set_aspect (nux_nid_t e, nux_f32_t aspect)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->aspect = aspect;
}
void
nux_camera_reset_aspect (nux_nid_t e, nux_viewport_t *viewport)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    nux_texture_t *t = nux_resource_get(NUX_RESOURCE_TEXTURE, viewport->target);
    NUX_ASSERT(t);
    c->aspect = (nux_f32_t)t->gpu.width / t->gpu.height;
}
void
nux_camera_set_ortho (nux_nid_t e, nux_b32_t ortho)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->ortho = ortho;
}
void
nux_camera_set_ortho_size (nux_nid_t e, nux_v2_t size)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->ortho_size = size;
}
nux_m4_t
nux_camera_get_projection (nux_nid_t e)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return nux_m4_identity());
    nux_m4_t proj;
    if (c->ortho)
    {
        proj = nux_m4_ortho(-c->ortho_size.x,
                            c->ortho_size.x,
                            -c->ortho_size.y,
                            c->ortho_size.y,
                            c->near,
                            c->far);
    }
    else
    {
        proj = nux_m4_perspective(
            nux_radian(c->fov), c->aspect, c->near, c->far);
    }
    return proj;
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
    nux_m4_t proj = nux_camera_get_projection(e);
    nux_m4_t vp   = nux_m4_mul(proj, view);
    nux_m4_t inv  = nux_m4_inv(vp);

    pos.y = 1 - pos.y; // convert screen to ndc space
    pos   = nux_v2_subs(nux_v2_muls(pos, 2), 1);
    return nux_v3_normalize(nux_m4_mulv3(inv, nux_v3(pos.x, pos.y, 1), 1));
}
void
nux_camera_set_render_mask (nux_nid_t n, nux_u32_t mask)
{
    nux_camera_t *c = nux_component_get(n, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->render_mask = mask;
}
nux_u32_t
nux_camera_get_render_mask (nux_nid_t n)
{
    nux_camera_t *c = nux_component_get(n, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return 0);
    return c->render_mask;
}
