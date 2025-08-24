#include "internal.h"

nux_b32_t
nux_intersect_ray_sphere (nux_ray_t r, nux_sphere_t s, nux_hit_t *hit)
{
    nux_v3_t  oc   = nux_v3_sub(r.p, s.p);
    nux_f32_t a    = nux_v3_dot(r.d, r.d);
    nux_f32_t b    = 2 * nux_v3_dot(oc, r.d);
    nux_f32_t c    = nux_v3_dot(oc, oc) - s.r * s.r;
    nux_f32_t desc = b * b - 4 * a * c;
    return desc > 0;
}
nux_b32_t
nux_intersect_segment_plane (nux_segment_t s, nux_plane_t p, nux_hit_t *hit)
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
        return NUX_FALSE;
    }
    nux_f32_t i = n / d;
    if (i < 0 || i > 1)
    {
        return NUX_FALSE;
    }
    if (hit)
    {
        hit->p = nux_v3_add(s.p0, nux_v3_muls(u, i));
    }
    return NUX_TRUE;
}
nux_b32_t
nux_intersect_ray_box (nux_ray_t r, nux_b3_t box, nux_hit_t *hit)
{
    // https://tavianator.com/cgit/dimension.git/tree/libdimension/bvh/bvh.c#n196
    nux_v3_t n_inv = nux_v3_inv(r.d);

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

    return tmax >= NUX_MAX(0, tmin) && tmin < NUX_FLT_MAX;
}
