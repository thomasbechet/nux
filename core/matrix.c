#include "internal.h"

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
