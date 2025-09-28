#include "internal.h"

nux_m4_t
nux_perspective (nux_f32_t fov,
                 nux_f32_t aspect_ratio,
                 nux_f32_t z_near,
                 nux_f32_t z_far)
{
    nux_m4_t  m;
    nux_f32_t y_scale  = 1.0f / nux_tan(fov / 2.0f);
    nux_f32_t x_scale  = y_scale / aspect_ratio;
    nux_f32_t near_far = z_near - z_far;

    m.data[0] = x_scale;
    m.data[1] = 0;
    m.data[2] = 0;
    m.data[3] = 0;

    m.data[4] = 0;
    m.data[5] = y_scale;
    m.data[6] = 0;
    m.data[7] = 0;

    m.data[8]  = 0;
    m.data[9]  = 0;
    m.data[10] = (z_far + z_near) / near_far;
    m.data[11] = -1;

    m.data[12] = 0;
    m.data[13] = 0;
    m.data[14] = (2 * z_far * z_near) / near_far;
    m.data[15] = 0;

    return m;
}
nux_m4_t
nux_ortho (nux_f32_t left,
           nux_f32_t right,
           nux_f32_t bottom,
           nux_f32_t top,
           nux_f32_t nnear,
           nux_f32_t far)
{
    nux_m4_t m;

    nux_f32_t rl = 1.0 / (right - left);
    nux_f32_t tb = 1.0 / (top - bottom);
    nux_f32_t fn = -1.0 / (far - nnear);

    m.data[0] = 2.0 * rl;
    m.data[1] = 0;
    m.data[2] = 0;
    m.data[3] = 0;

    m.data[4] = 0;
    m.data[5] = 2.0 * tb;
    m.data[6] = 0;
    m.data[7] = 0;

    m.data[8]  = 0;
    m.data[9]  = 0;
    m.data[10] = 2.0 * fn;
    m.data[11] = 0;

    m.data[12] = -(right + left) * rl;
    m.data[13] = -(top + bottom) * tb;
    m.data[14] = (far + nnear) * fn;
    m.data[15] = 1;

    return m;
}
nux_m4_t
nux_lookat (nux_v3_t eye, nux_v3_t center, nux_v3_t up)
{
    nux_v3_t f = nux_v3(center.x - eye.x, center.y - eye.y, center.z - eye.z);
    f          = nux_v3_normalize(f);

    nux_v3_t u = nux_v3_normalize(up);

    nux_v3_t s = nux_v3_cross(f, u);
    s          = nux_v3_normalize(s);

    nux_v3_t u_prime = nux_v3_cross(s, f);

    nux_m4_t m;
    m.data[0] = s.x;
    m.data[1] = u_prime.x;
    m.data[2] = -f.x;
    m.data[3] = 0;

    m.data[4] = s.y;
    m.data[5] = u_prime.y;
    m.data[6] = -f.y;
    m.data[7] = 0;

    m.data[8]  = s.z;
    m.data[9]  = u_prime.z;
    m.data[10] = -f.z;
    m.data[11] = 0;

    m.data[12] = -s.x * eye.x - s.y * eye.y - s.z * eye.z;
    m.data[13] = -u_prime.x * eye.x - u_prime.y * eye.y - u_prime.z * eye.z;
    m.data[14] = f.x * eye.x + f.y * eye.y + f.z * eye.z;
    m.data[15] = 1;
    return m;
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
nux_camera_add (nux_eid_t e)
{
    nux_camera_t *c = nux_ecs_add(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->fov      = 60;
    c->near     = 0.1;
    c->far      = 200;
    c->viewport = nux_b2i_xywh(0, 0, 100, 100);
}
void
nux_camera_remove (nux_eid_t e)
{
    nux_ecs_remove(e, NUX_COMPONENT_CAMERA);
}
void
nux_camera_set_fov (nux_eid_t e, nux_f32_t fov)
{
    nux_camera_t *c = nux_ecs_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->fov = fov;
}
void
nux_camera_set_near (nux_eid_t e, nux_f32_t near)
{
    nux_camera_t *c = nux_ecs_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->near = near;
}
void
nux_camera_set_far (nux_eid_t e, nux_f32_t far)
{
    nux_camera_t *c = nux_ecs_get(e, NUX_COMPONENT_CAMERA);
    NUX_CHECK(c, return);
    c->far = far;
}
