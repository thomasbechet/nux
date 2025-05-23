#include "internal.h"

nux_q4_t
nux_q4 (nux_f32_t x, nux_f32_t y, nux_f32_t z, nux_f32_t w)
{
    nux_q4_t q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}
nux_q4_t
nux_q4_identity (void)
{
    return nux_q4(0, 0, 0, 1);
}
nux_v4_t
nux_q4_vec4 (nux_q4_t a)
{
    return nux_v4(a.x, a.y, a.z, a.w);
}
nux_f32_t
nux_q4_norm (nux_q4_t a)
{
    return nux_v4_norm(nux_q4_vec4(a));
}
nux_q4_t
nux_q4_axis (nux_v3_t axis, nux_f32_t angle)
{
    nux_f32_t a = angle * 0.5;
    nux_f32_t c = nux_cos(a);
    nux_f32_t s = nux_sin(a);

    nux_v3_t k = nux_v3_normalize(axis);

    return nux_q4(s * k.x, s * k.y, s * k.z, c);
}
nux_q4_t
nux_q4_mul (nux_q4_t a, nux_q4_t b)
{
    nux_q4_t q;
    q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
    q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    return q;
}
nux_v3_t
nux_q4_mulv3 (nux_q4_t a, nux_v3_t v)
{
    nux_v3_t u = nux_v3(a.x, a.y, a.z);

    nux_v3_t v1 = nux_v3_muls(u, 2 * nux_v3_dot(u, v));
    nux_v3_t v2 = nux_v3_muls(v, a.w * a.w - nux_v3_dot(u, u));
    nux_v3_t v3 = nux_v3_muls(nux_v3_cross(u, v), 2 * a.w);

    return nux_v3_add(v1, nux_v3_add(v2, v3));
}
nux_q4_t
nux_q4_mul_axis (nux_q4_t q, nux_v3_t axis, nux_f32_t angle)
{
    return nux_q4_mul(q, nux_q4_axis(axis, angle));
}
nux_m3_t
nux_q4_mat3 (nux_q4_t q)
{
    nux_f32_t norm = nux_q4_norm(q);
    nux_f32_t s    = norm > 0.0 ? 2.0 / norm : 0.0;

    nux_f32_t x = q.x;
    nux_f32_t y = q.y;
    nux_f32_t z = q.z;
    nux_f32_t w = q.w;

    nux_f32_t xx = s * x * x;
    nux_f32_t xy = s * x * y;
    nux_f32_t wx = s * w * x;
    nux_f32_t yy = s * y * y;
    nux_f32_t yz = s * y * z;
    nux_f32_t wy = s * w * y;
    nux_f32_t zz = s * z * z;
    nux_f32_t xz = s * x * z;
    nux_f32_t wz = s * w * z;

    nux_m3_t m;

    m.x1 = 1.0 - yy - zz;
    m.y2 = 1.0 - xx - zz;
    m.z3 = 1.0 - xx - yy;

    m.x2 = xy + wz;
    m.y3 = yz + wx;
    m.z1 = xz + wy;

    m.y1 = xy - wz;
    m.z2 = yz - wx;
    m.x3 = xz - wy;

    return m;
}
nux_m4_t
nux_q4_mat4 (nux_q4_t q)
{
    nux_f32_t norm = nux_q4_norm(q);
    nux_f32_t s    = norm > 0.0 ? 2.0 / norm : 0.0;

    nux_f32_t x = q.x;
    nux_f32_t y = q.y;
    nux_f32_t z = q.z;
    nux_f32_t w = q.w;

    nux_f32_t xx = s * x * x;
    nux_f32_t xy = s * x * y;
    nux_f32_t wx = s * w * x;
    nux_f32_t yy = s * y * y;
    nux_f32_t yz = s * y * z;
    nux_f32_t wy = s * w * y;
    nux_f32_t zz = s * z * z;
    nux_f32_t xz = s * x * z;
    nux_f32_t wz = s * w * z;

    nux_m4_t m;

    m.x1 = 1.0 - yy - zz;
    m.y2 = 1.0 - xx - zz;
    m.z3 = 1.0 - xx - yy;

    m.x2 = xy + wz;
    m.y3 = yz + wx;
    m.z1 = xz + wy;

    m.y1 = xy - wz;
    m.z2 = yz - wx;
    m.x3 = xz - wy;

    m.x4 = 0;
    m.y4 = 0;
    m.z4 = 0;
    m.w1 = 0;
    m.w2 = 0;
    m.w3 = 0;
    m.w4 = 1;

    return m;
}
nux_m4_t
nux_q4_mulm4 (nux_q4_t a, nux_m4_t m)
{
    // TODO: allow mat4 multiplication on rotation only
    return nux_m4_mul(nux_q4_mat4(a), m);
}
