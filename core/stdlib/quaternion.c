#include "stdlib.h"

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
nux_q4_from_m3 (nux_m3_t rot)
{
    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    nux_q4_t  q;
    nux_f32_t trace = rot.x1 + rot.y2 + rot.z3;
    if (trace > 0)
    {
        nux_f32_t s = 0.5f / nux_sqrt(trace + 1.0f);
        q.w         = 0.25f / s;
        q.x         = (rot.z2 - rot.y3) * s;
        q.y         = (rot.x3 - rot.z1) * s;
        q.z         = (rot.y1 - rot.x2) * s;
    }
    else
    {
        if (rot.x1 > rot.y2 && rot.x1 > rot.z3)
        {
            nux_f32_t s = 2.0f * nux_sqrt(1.0f + rot.x1 - rot.y2 - rot.z3);
            q.w         = (rot.z2 - rot.y3) / s;
            q.x         = 0.25f * s;
            q.y         = (rot.x2 + rot.y1) / s;
            q.z         = (rot.x3 + rot.z1) / s;
        }
        else if (rot.y2 > rot.z3)
        {
            nux_f32_t s = 2.0f * nux_sqrt(1.0f + rot.y2 - rot.x1 - rot.z3);
            q.w         = (rot.x3 - rot.z1) / s;
            q.x         = (rot.x2 + rot.y1) / s;
            q.y         = 0.25f * s;
            q.z         = (rot.y3 + rot.z2) / s;
        }
        else
        {
            nux_f32_t s = 2.0f * nux_sqrt(1.0f + rot.z3 - rot.x1 - rot.y2);
            q.w         = (rot.y1 - rot.x2) / s;
            q.x         = (rot.x3 + rot.z1) / s;
            q.y         = (rot.y3 + rot.z2) / s;
            q.z         = 0.25f * s;
        }
    }

    return q;
}
nux_q4_t
nux_q4_euler (nux_v3_t euler)
{
    nux_f32_t c_pitch = nux_cos(euler.x * 0.5);
    nux_f32_t s_pitch = nux_sin(euler.x * 0.5);
    nux_f32_t c_yaw   = nux_cos(euler.y * 0.5);
    nux_f32_t s_yaw   = nux_sin(euler.y * 0.5);
    nux_f32_t c_roll  = nux_cos(euler.z * 0.5);
    nux_f32_t s_roll  = nux_sin(euler.z * 0.5);

    nux_q4_t q;
    q.w = c_pitch * c_yaw * c_roll + s_pitch * s_yaw * s_roll;
    q.x = s_pitch * c_yaw * c_roll - c_pitch * s_yaw * s_roll;
    q.y = c_pitch * s_yaw * c_roll + s_pitch * c_yaw * s_roll;
    q.z = c_pitch * c_yaw * s_roll - s_pitch * s_yaw * c_roll;
    return q;
}
nux_v3_t
nux_q4_to_euler (nux_q4_t q)
{
    nux_v3_t angles;

    // roll (x-axis rotation)
    nux_f32_t sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    nux_f32_t cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.x            = nux_atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    nux_f32_t sinp = nux_sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
    nux_f32_t cosp = nux_sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
    angles.y       = 2 * nux_atan2(sinp, cosp) - M_PI / 2;

    // yaw (z-axis rotation)
    nux_f32_t siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    nux_f32_t cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.z            = nux_atan2(siny_cosp, cosy_cosp);

    return angles;
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
