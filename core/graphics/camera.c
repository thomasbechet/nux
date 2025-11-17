#include "internal.h"

void
nux_camera_add (nux_nid_t e, void *data)
{
    nux_camera_t *c = data;
    c->fov          = 60;
    c->near         = 0.1;
    c->far          = 1000;
    c->aspect       = 16. / 9.;
    c->ortho_size   = nux_v2s(1);
    c->ortho        = false;
    c->render_mask  = NUX_LAYER_DEFAULT;
}
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
nux_camera_set_fov (nux_nid_t e, nux_f32_t fov)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->fov = fov;
}
void
nux_camera_set_near (nux_nid_t e, nux_f32_t near)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->near = near;
}
void
nux_camera_set_far (nux_nid_t e, nux_f32_t far)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->far = far;
}
void
nux_camera_set_aspect (nux_nid_t e, nux_f32_t aspect)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->aspect = aspect;
}
void
nux_camera_set_ortho (nux_nid_t e, nux_b32_t ortho)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->ortho = ortho;
}
void
nux_camera_set_ortho_size (nux_nid_t e, nux_v2_t size)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->ortho_size = size;
}
nux_m4_t
nux_camera_projection (nux_nid_t e)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return nux_m4_identity());
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
    nux_check(c, return nux_v3_zero());
    nux_check(nux_node_has(e, NUX_COMPONENT_TRANSFORM), return nux_v3_zero());

    nux_m4_t global_matrix = nux_transform_matrix(e);

    nux_v3_t eye    = nux_m4_mulv3(global_matrix, nux_v3_zero(), 1);
    nux_v3_t center = nux_m4_mulv3(global_matrix, nux_v3_forward(), 1);
    nux_v3_t up     = nux_m4_mulv3(global_matrix, nux_v3_up(), 1);

    nux_m4_t view = nux_m4_lookat(eye, center, up);
    nux_m4_t proj = nux_camera_projection(e);
    nux_m4_t vp   = nux_m4_mul(proj, view);
    nux_m4_t inv  = nux_m4_inv(vp);

    pos.y = 1 - pos.y; // convert screen to ndc space
    pos   = nux_v2_subs(nux_v2_muls(pos, 2), 1);

    nux_v4_t p = nux_m4_mulv(inv, nux_v4(pos.x, pos.y, 0, 1));
    p          = nux_v4_divs(p, p.w);

    return nux_v3(p.x, p.y, p.z);
}
void
nux_camera_set_render_mask (nux_nid_t n, nux_u32_t mask)
{
    nux_camera_t *c = nux_component_get(n, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->render_mask = mask;
}
nux_u32_t
nux_camera_render_mask (nux_nid_t n)
{
    nux_camera_t *c = nux_component_get(n, NUX_COMPONENT_CAMERA);
    nux_check(c, return 0);
    return c->render_mask;
}
void
nux_camera_set_viewport (nux_nid_t e, nux_b2i_t viewport)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->viewport = viewport;
}
nux_b2i_t
nux_camera_viewport (nux_nid_t e)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return nux_b2i(0, 0, 0, 0));
    return c->viewport;
}
void
nux_camera_set_target (nux_nid_t e, nux_rid_t target)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return);
    c->target = target;
}
nux_rid_t
nux_camera_target (nux_nid_t e)
{
    nux_camera_t *c = nux_component_get(e, NUX_COMPONENT_CAMERA);
    nux_check(c, return NUX_NULL);
    return c->target;
}
nux_v2_t
nux_camera_to_global (nux_nid_t e, nux_v2_t coord)
{
}
nux_v2_t
nux_camera_to_local (nux_nid_t e, nux_v2_t coord)
{
}
