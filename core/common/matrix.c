#include "common.h"

nux_m3_t
nux_m3_zero (void)
{
    nux_m3_t m;
    for (nux_u32_t i = 0; i < NUX_M3_SIZE; ++i)
    {
        m.data[i] = 0;
    }
    return m;
}
nux_m3_t
nux_m3_identity (void)
{
    nux_m3_t m = nux_m3_zero();
    m.x1       = 1;
    m.y2       = 1;
    m.z3       = 1;
    return m;
}
nux_m3_t
nux_m3_axis (nux_v3_t x, nux_v3_t y, nux_v3_t z)
{
    nux_m3_t m;
    m.x1 = x.x;
    m.y1 = x.y;
    m.z1 = x.z;
    m.x2 = y.x;
    m.y2 = y.y;
    m.z2 = y.z;
    m.x3 = z.x;
    m.y3 = z.y;
    m.z3 = z.z;
    return m;
}
nux_m3_t
nux_m3_translate (nux_v2_t v)
{
    nux_m3_t m = nux_m3_identity();
    m.z1       = v.x;
    m.z2       = v.y;
    return m;
}
nux_m3_t
nux_m3_scale (nux_v2_t v)
{
    nux_m3_t m = nux_m3_identity();
    m.x1       = v.x;
    m.y2       = v.y;
    return m;
}
nux_m3_t
nux_m3_mul (nux_m3_t a, nux_m3_t b)
{
    nux_m3_t m = nux_m3_zero();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                m.data[j * 3 + i] += a.data[k * 3 + i] * b.data[j * 3 + k];
            }
        }
    }
    return m;
}

nux_m4_t
nux_m4 (const nux_f32_t *p)
{
    nux_m4_t m;
    nux_memcpy(m.data, p, sizeof(*p) * NUX_M4_SIZE);
    return m;
}
nux_m4_t
nux_m4_zero (void)
{
    nux_m4_t m;
    for (nux_u32_t i = 0; i < NUX_M4_SIZE; ++i)
    {
        m.data[i] = 0;
    }
    return m;
}
nux_m4_t
nux_m4_identity (void)
{
    nux_m4_t m = nux_m4_zero();
    m.x1       = 1;
    m.y2       = 1;
    m.z3       = 1;
    m.w4       = 1;
    return m;
}
nux_m4_t
nux_m4_inv (nux_m4_t m)
{
    nux_m4_t  inv;
    nux_f32_t det;

    inv.data2d[0][0] = m.data2d[1][1] * m.data2d[2][2] * m.data2d[3][3]
                       - m.data2d[1][1] * m.data2d[3][2] * m.data2d[2][3]
                       - m.data2d[1][2] * m.data2d[2][1] * m.data2d[3][3]
                       + m.data2d[1][2] * m.data2d[3][1] * m.data2d[2][3]
                       + m.data2d[1][3] * m.data2d[2][1] * m.data2d[3][2]
                       - m.data2d[1][3] * m.data2d[3][1] * m.data2d[2][2];

    inv.data2d[0][1] = -m.data2d[0][1] * m.data2d[2][2] * m.data2d[3][3]
                       + m.data2d[0][1] * m.data2d[3][2] * m.data2d[2][3]
                       + m.data2d[0][2] * m.data2d[2][1] * m.data2d[3][3]
                       - m.data2d[0][2] * m.data2d[3][1] * m.data2d[2][3]
                       - m.data2d[0][3] * m.data2d[2][1] * m.data2d[3][2]
                       + m.data2d[0][3] * m.data2d[3][1] * m.data2d[2][2];

    inv.data2d[0][2] = m.data2d[0][1] * m.data2d[1][2] * m.data2d[3][3]
                       - m.data2d[0][1] * m.data2d[3][2] * m.data2d[1][3]
                       - m.data2d[0][2] * m.data2d[1][1] * m.data2d[3][3]
                       + m.data2d[0][2] * m.data2d[3][1] * m.data2d[1][3]
                       + m.data2d[0][3] * m.data2d[1][1] * m.data2d[3][2]
                       - m.data2d[0][3] * m.data2d[3][1] * m.data2d[1][2];

    inv.data2d[0][3] = -m.data2d[0][1] * m.data2d[1][2] * m.data2d[2][3]
                       + m.data2d[0][1] * m.data2d[2][2] * m.data2d[1][3]
                       + m.data2d[0][2] * m.data2d[1][1] * m.data2d[2][3]
                       - m.data2d[0][2] * m.data2d[2][1] * m.data2d[1][3]
                       - m.data2d[0][3] * m.data2d[1][1] * m.data2d[2][2]
                       + m.data2d[0][3] * m.data2d[2][1] * m.data2d[1][2];

    inv.data2d[1][0] = -m.data2d[1][0] * m.data2d[2][2] * m.data2d[3][3]
                       + m.data2d[1][0] * m.data2d[3][2] * m.data2d[2][3]
                       + m.data2d[1][2] * m.data2d[2][0] * m.data2d[3][3]
                       - m.data2d[1][2] * m.data2d[3][0] * m.data2d[2][3]
                       - m.data2d[1][3] * m.data2d[2][0] * m.data2d[3][2]
                       + m.data2d[1][3] * m.data2d[3][0] * m.data2d[2][2];

    inv.data2d[1][1] = m.data2d[0][0] * m.data2d[2][2] * m.data2d[3][3]
                       - m.data2d[0][0] * m.data2d[3][2] * m.data2d[2][3]
                       - m.data2d[0][2] * m.data2d[2][0] * m.data2d[3][3]
                       + m.data2d[0][2] * m.data2d[3][0] * m.data2d[2][3]
                       + m.data2d[0][3] * m.data2d[2][0] * m.data2d[3][2]
                       - m.data2d[0][3] * m.data2d[3][0] * m.data2d[2][2];

    inv.data2d[1][2] = -m.data2d[0][0] * m.data2d[1][2] * m.data2d[3][3]
                       + m.data2d[0][0] * m.data2d[3][2] * m.data2d[1][3]
                       + m.data2d[0][2] * m.data2d[1][0] * m.data2d[3][3]
                       - m.data2d[0][2] * m.data2d[3][0] * m.data2d[1][3]
                       - m.data2d[0][3] * m.data2d[1][0] * m.data2d[3][2]
                       + m.data2d[0][3] * m.data2d[3][0] * m.data2d[1][2];

    inv.data2d[1][3] = m.data2d[0][0] * m.data2d[1][2] * m.data2d[2][3]
                       - m.data2d[0][0] * m.data2d[2][2] * m.data2d[1][3]
                       - m.data2d[0][2] * m.data2d[1][0] * m.data2d[2][3]
                       + m.data2d[0][2] * m.data2d[2][0] * m.data2d[1][3]
                       + m.data2d[0][3] * m.data2d[1][0] * m.data2d[2][2]
                       - m.data2d[0][3] * m.data2d[2][0] * m.data2d[1][2];

    inv.data2d[2][0] = m.data2d[1][0] * m.data2d[2][1] * m.data2d[3][3]
                       - m.data2d[1][0] * m.data2d[3][1] * m.data2d[2][3]
                       - m.data2d[1][1] * m.data2d[2][0] * m.data2d[3][3]
                       + m.data2d[1][1] * m.data2d[3][0] * m.data2d[2][3]
                       + m.data2d[1][3] * m.data2d[2][0] * m.data2d[3][1]
                       - m.data2d[1][3] * m.data2d[3][0] * m.data2d[2][1];

    inv.data2d[2][1] = -m.data2d[0][0] * m.data2d[2][1] * m.data2d[3][3]
                       + m.data2d[0][0] * m.data2d[3][1] * m.data2d[2][3]
                       + m.data2d[0][1] * m.data2d[2][0] * m.data2d[3][3]
                       - m.data2d[0][1] * m.data2d[3][0] * m.data2d[2][3]
                       - m.data2d[0][3] * m.data2d[2][0] * m.data2d[3][1]
                       + m.data2d[0][3] * m.data2d[3][0] * m.data2d[2][1];

    inv.data2d[2][2] = m.data2d[0][0] * m.data2d[1][1] * m.data2d[3][3]
                       - m.data2d[0][0] * m.data2d[3][1] * m.data2d[1][3]
                       - m.data2d[0][1] * m.data2d[1][0] * m.data2d[3][3]
                       + m.data2d[0][1] * m.data2d[3][0] * m.data2d[1][3]
                       + m.data2d[0][3] * m.data2d[1][0] * m.data2d[3][1]
                       - m.data2d[0][3] * m.data2d[3][0] * m.data2d[1][1];

    inv.data2d[2][3] = -m.data2d[0][0] * m.data2d[1][1] * m.data2d[2][3]
                       + m.data2d[0][0] * m.data2d[2][1] * m.data2d[1][3]
                       + m.data2d[0][1] * m.data2d[1][0] * m.data2d[2][3]
                       - m.data2d[0][1] * m.data2d[2][0] * m.data2d[1][3]
                       - m.data2d[0][3] * m.data2d[1][0] * m.data2d[2][1]
                       + m.data2d[0][3] * m.data2d[2][0] * m.data2d[1][1];

    inv.data2d[3][0] = -m.data2d[1][0] * m.data2d[2][1] * m.data2d[3][2]
                       + m.data2d[1][0] * m.data2d[3][1] * m.data2d[2][2]
                       + m.data2d[1][1] * m.data2d[2][0] * m.data2d[3][2]
                       - m.data2d[1][1] * m.data2d[3][0] * m.data2d[2][2]
                       - m.data2d[1][2] * m.data2d[2][0] * m.data2d[3][1]
                       + m.data2d[1][2] * m.data2d[3][0] * m.data2d[2][1];

    inv.data2d[3][1] = m.data2d[0][0] * m.data2d[2][1] * m.data2d[3][2]
                       - m.data2d[0][0] * m.data2d[3][1] * m.data2d[2][2]
                       - m.data2d[0][1] * m.data2d[2][0] * m.data2d[3][2]
                       + m.data2d[0][1] * m.data2d[3][0] * m.data2d[2][2]
                       + m.data2d[0][2] * m.data2d[2][0] * m.data2d[3][1]
                       - m.data2d[0][2] * m.data2d[3][0] * m.data2d[2][1];

    inv.data2d[3][2] = -m.data2d[0][0] * m.data2d[1][1] * m.data2d[3][2]
                       + m.data2d[0][0] * m.data2d[3][1] * m.data2d[1][2]
                       + m.data2d[0][1] * m.data2d[1][0] * m.data2d[3][2]
                       - m.data2d[0][1] * m.data2d[3][0] * m.data2d[1][2]
                       - m.data2d[0][2] * m.data2d[1][0] * m.data2d[3][1]
                       + m.data2d[0][2] * m.data2d[3][0] * m.data2d[1][1];

    inv.data2d[3][3] = m.data2d[0][0] * m.data2d[1][1] * m.data2d[2][2]
                       - m.data2d[0][0] * m.data2d[2][1] * m.data2d[1][2]
                       - m.data2d[0][1] * m.data2d[1][0] * m.data2d[2][2]
                       + m.data2d[0][1] * m.data2d[2][0] * m.data2d[1][2]
                       + m.data2d[0][2] * m.data2d[1][0] * m.data2d[2][1]
                       - m.data2d[0][2] * m.data2d[2][0] * m.data2d[1][1];

    det = m.data2d[0][0] * inv.data2d[0][0] + m.data2d[1][0] * inv.data2d[0][1]
          + m.data2d[2][0] * inv.data2d[0][2]
          + m.data2d[3][0] * inv.data2d[0][3];

    if (det == 0)
    {
        // Cannot inverse
    }

    det = 1.f / det;

    for (nux_u32_t x = 0; x < 4; x++)
    {
        for (nux_u32_t y = 0; y < 4; y++)
        {
            m.data2d[x][y] = inv.data2d[x][y] * det;
        }
    }

    return m;
}
nux_m4_t
nux_m4_translate (nux_v3_t v)
{
    nux_m4_t m = nux_m4_identity();
    m.w1       = v.x;
    m.w2       = v.y;
    m.w3       = v.z;
    return m;
}
nux_m4_t
nux_m4_scale (nux_v3_t v)
{
    nux_m4_t m = nux_m4_identity();
    m.x1       = v.x;
    m.y2       = v.y;
    m.z3       = v.z;
    return m;
}
nux_m4_t
nux_m4_rotate_y (nux_f32_t angle)
{
    nux_m4_t m = nux_m4_identity();
    m.x1       = nux_cos(angle);
    m.z1       = nux_sin(angle);
    m.x3       = -nux_sin(angle);
    m.z3       = nux_cos(angle);
    return m;
}
nux_m4_t
nux_m4_mul (nux_m4_t a, nux_m4_t b)
{
    nux_m4_t m = nux_m4_zero();
    for (nux_u32_t i = 0; i < 4; ++i)
    {
        for (nux_u32_t j = 0; j < 4; ++j)
        {
            for (nux_u32_t k = 0; k < 4; ++k)
            {
                m.data[j * 4 + i] += a.data[k * 4 + i] * b.data[j * 4 + k];
            }
        }
    }
    return m;
}
nux_v4_t
nux_m4_mulv (nux_m4_t m, nux_v4_t v)
{
    nux_v4_t ret;
    ret.x = m.x1 * v.x + m.y1 * v.y + m.z1 * v.z + m.w1 * v.w;
    ret.y = m.x2 * v.x + m.y2 * v.y + m.z2 * v.z + m.w2 * v.w;
    ret.z = m.x3 * v.x + m.y3 * v.y + m.z3 * v.z + m.w3 * v.w;
    ret.w = m.x4 * v.x + m.y4 * v.y + m.z4 * v.z + m.w4 * v.w;
    return ret;
}
nux_v3_t
nux_m4_mulv3 (nux_m4_t a, nux_v3_t v, nux_f32_t w)
{
    nux_v4_t v4 = nux_v4(v.x, v.y, v.z, w);
    v4          = nux_m4_mulv(a, v4);
    return nux_v3(v4.x, v4.y, v4.z);
}
nux_m4_t
nux_m4_trs (nux_v3_t t, nux_q4_t r, nux_v3_t s)
{
    return nux_m4_mul(nux_m4_translate(t), nux_q4_mulm4(r, nux_m4_scale(s)));
}
void
nux_m4_trs_decompose (nux_m4_t m, nux_v3_t *t, nux_q4_t *r, nux_v3_t *s)
{
    // Expect transform without shearing
    nux_v3_t  translation = nux_v3(m.w1, m.w2, m.w3);
    nux_f32_t sx          = nux_v3_norm(nux_v3(m.x1, m.x2, m.x3));
    nux_f32_t sy          = nux_v3_norm(nux_v3(m.y1, m.y2, m.y3));
    nux_f32_t sz          = nux_v3_norm(nux_v3(m.z1, m.z2, m.z3));
    nux_v3_t  scale       = nux_v3(sx, sy, sz);
    if (t)
    {
        *t = translation;
    }
    if (r)
    {
    }
    if (s)
    {
        *s = scale;
    }
}
nux_m4_t
nux_m4_perspective (nux_f32_t fov,
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
nux_m4_ortho (nux_f32_t left,
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
nux_m4_lookat (nux_v3_t eye, nux_v3_t center, nux_v3_t up)
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
