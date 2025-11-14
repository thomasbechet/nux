#include "internal.h"

nux_b32_t
nux_intersect_ray_sphere (nux_ray_t r, nux_sphere_t s, nux_f32_t *t0)
{
    nux_v3_t  m = nux_v3_sub(r.p, s.p);
    nux_f32_t b = nux_v3_dot(m, r.d);
    nux_f32_t c = nux_v3_dot(m, m) - s.r * s.r;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
    {
        return false;
    }
    nux_f32_t discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
    {
        return false;
    }

    // Ray now found to intersect sphere, compute smallest t value of
    // intersection
    nux_f32_t t = -b - nux_sqrt(discr);

    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    if (t0)
    {
        *t0 = t;
    }

    return true;
}
nux_b32_t
nux_intersect_segment_plane (nux_segment_t s, nux_plane_t p, nux_v3_t *i0)
{
    nux_v3_t  u = nux_v3_sub(s.p1, s.p0);
    nux_v3_t  w = nux_v3_sub(s.p0, p.p);
    nux_f32_t d = nux_v3_dot(p.n, u);
    nux_f32_t n = -nux_v3_dot(p.n, w);
    if (NUX_ABS(d) < NUX_F32_EPSILON)
    {
        if (n == 0)
        {
            // segment in plane
        }
        return false;
    }
    nux_f32_t i = n / d;
    if (i < 0 || i > 1)
    {
        return false;
    }
    if (i0)
    {
        *i0 = nux_v3_add(s.p0, nux_v3_muls(u, i));
    }
    return true;
}
nux_b32_t
nux_intersect_ray_box (nux_ray_t r, nux_b3_t box, nux_f32_t *t0, nux_f32_t *t1)
{
    // https://tavianator.com/cgit/dimension.git/tree/libdimension/bvh/bvh.c#n196
    nux_v3_t n_inv;
    n_inv.x = r.d.x ? 1 / r.d.x : 0;
    n_inv.y = r.d.y ? 1 / r.d.y : 0;
    n_inv.z = r.d.z ? 1 / r.d.z : 0;

    nux_f32_t tx1 = (box.min.x - r.p.x) * n_inv.x;
    nux_f32_t tx2 = (box.max.x - r.p.x) * n_inv.x;

    nux_f32_t tmin = NUX_MIN(tx1, tx2);
    nux_f32_t tmax = NUX_MAX(tx1, tx2);

    nux_f32_t ty1 = (box.min.y - r.p.y) * n_inv.y;
    nux_f32_t ty2 = (box.max.y - r.p.y) * n_inv.y;

    tmin = NUX_MAX(tmin, NUX_MIN(ty1, ty2));
    tmax = NUX_MIN(tmax, NUX_MAX(ty1, ty2));

    nux_f32_t tz1 = (box.min.z - r.p.z) * n_inv.z;
    nux_f32_t tz2 = (box.max.z - r.p.z) * n_inv.z;

    tmin = NUX_MAX(tmin, NUX_MIN(tz1, tz2));
    tmax = NUX_MIN(tmax, NUX_MAX(tz1, tz2));

    nux_b32_t hit = (tmax >= NUX_MAX(0, tmin)) && (tmin < NUX_FLT_MAX);

    if (hit)
    {
        if (t0)
        {
            *t0 = tmin;
        }
        if (t1)
        {
            *t1 = tmax;
        }
        return true;
    }

    return false;
}
