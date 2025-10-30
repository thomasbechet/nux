#include "internal.h"
#include <stdlib.h>

#define NUX_B3_IMPL(name, type)                                         \
    nux_##name##_t nux_##name(type min, type max)                       \
    {                                                                   \
        NUX_ASSERT(max.x >= min.x && max.y >= min.y && max.z >= min.z); \
        nux_##name##_t b;                                               \
        b.min = min;                                                    \
        b.max = max;                                                    \
        return b;                                                       \
    }

nux_u32_t
nux_log2 (nux_u32_t n)
{
    nux_u32_t result = 0;
    n                = n >> 1;
    while (n != 0)
    {
        result++;
        n = n >> 1;
    }
    return result;
}
nux_f32_t
nux_log10 (nux_f32_t x)
{
#ifdef NUX_BUILD_STDLIB
    return log10f(x);
#endif
}
nux_f32_t
nux_fabs (nux_f32_t f)
{
#ifdef NUX_BUILD_STDLIB
    return fabsf(f);
#endif
}
nux_f32_t
nux_floor (nux_f32_t f)
{
#ifdef NUX_BUILD_STDLIB
    return floorf(f);
#endif
}
nux_f32_t
nux_radian (nux_f32_t d)
{
    return d * (NUX_PI / 180.0f);
}
nux_f32_t
nux_sqrt (nux_f32_t x)
{
#ifdef NUX_BUILD_STDLIB
    return sqrtf(x);
#endif
}
nux_f32_t
nux_pow (nux_f32_t b, nux_f32_t e)
{
#ifdef NUX_BUILD_STDLIB
    return powf(b, e);
#endif
}
nux_f32_t
nux_cos (nux_f32_t x)
{
#ifdef NUX_BUILD_STDLIB
    return cosf(x);
#endif
}
nux_f32_t
nux_sin (nux_f32_t x)
{
#ifdef NUX_BUILD_STDLIB
    return sinf(x);
#endif
}
nux_f32_t
nux_tan (nux_f32_t x)
{
#ifdef NUX_BUILD_STDLIB
    return tanf(x);
#endif
}
nux_f32_t
nux_exp (nux_f32_t x)
{
#ifdef NUX_BUILD_STDLIB
    return expf(x);
#endif
}
nux_f32_t
nux_atan2 (nux_f32_t x, nux_f32_t y)
{
#ifdef NUX_BUILD_STDLIB
    return atan2f(x, y);
#endif
}

nux_b2i_t
nux_b2i_min_max (nux_v2i_t min, nux_v2i_t max)
{
    NUX_ASSERT(max.x >= min.x && max.y >= min.y);
    nux_b2i_t ret;
    ret.x = min.x;
    ret.y = min.y;
    ret.w = max.x - min.x;
    ret.h = max.y - min.y;
    return ret;
}
nux_b2i_t
nux_b2i (nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h)
{
    NUX_ASSERT(w && h);
    nux_b2i_t ret;
    ret.x = x;
    ret.y = y;
    ret.w = w;
    ret.h = h;
    return ret;
}
nux_b2i_t
nux_b2i_translate (nux_b2i_t b, nux_v2i_t t)
{
    b.x += t.x;
    b.y += t.y;
    return b;
}
nux_b2i_t
nux_b2i_moveto (nux_b2i_t b, nux_v2i_t p)
{
    b.x = p.x;
    b.y = p.y;
    return b;
}
nux_v2u_t
nux_b2i_size (nux_b2i_t b)
{
    return nux_v2u(b.w, b.h);
}
nux_b32_t
nux_b2i_containsi (nux_b2i_t b, nux_v2i_t p)
{
    return (p.x >= b.x && p.x < (b.x + b.w) && p.y >= b.y && p.y < (b.y + b.h));
}
nux_b32_t
nux_b2i_contains (nux_b2i_t b, nux_v2_t p)
{
    return nux_b2i_containsi(b, nux_v2i(p.x, p.y));
}
nux_b2i_t
nux_b2i_merge (nux_b2i_t a, nux_b2i_t b)
{
    nux_v2i_t tla = nux_b2i_tl(a);
    nux_v2i_t tlb = nux_b2i_tl(b);
    nux_v2i_t bra = nux_b2i_br(a);
    nux_v2i_t brb = nux_b2i_br(b);
    return nux_b2i_min_max(nux_v2i_min(tla, tlb), nux_v2i_max(bra, brb));
}
nux_i32_t
nux_b2i_top (nux_b2i_t b)
{
    return b.y;
}
nux_i32_t
nux_b2i_bottom (nux_b2i_t b)
{
    return b.y + b.h;
}
nux_i32_t
nux_b2i_left (nux_b2i_t b)
{
    return b.x;
}
nux_i32_t
nux_b2i_right (nux_b2i_t b)
{
    return b.x + b.w;
}
nux_v2i_t
nux_b2i_tl (nux_b2i_t b)
{
    return nux_v2i(b.x, b.y);
}
nux_v2i_t
nux_b2i_br (nux_b2i_t b)
{
    return nux_v2i(b.x + b.w, b.y + b.h);
}

void
nux_qsort (void     *base,
           nux_u32_t n,
           nux_u32_t size,
           nux_i32_t (*compare)(const void *a, const void *b))
{
#ifdef NUX_BUILD_STDLIB
    return qsort(base, n, size, compare);
#endif
}

NUX_B3_IMPL(b3, nux_v3_t);
NUX_B3_IMPL(b3i, nux_v3i_t);
