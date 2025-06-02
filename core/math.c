#include "internal.h"

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
#ifdef NUX_STDLIB
    return log10f(x);
#endif
}
nux_f32_t
nux_fabs (nux_f32_t f)
{
#ifdef NUX_STDLIB
    return fabsf(f);
#endif
}
nux_f32_t
nux_floor (nux_f32_t f)
{
#ifdef NUX_STDLIB
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
#ifdef NUX_STDLIB
    return sqrtf(x);
#endif
}
nux_f32_t
nux_pow (nux_f32_t b, nux_f32_t e)
{
#ifdef NUX_STDLIB
    return powf(b, e);
#endif
}
nux_f32_t
nux_cos (nux_f32_t x)
{
#ifdef NUX_STDLIB
    return cosf(x);
#endif
}
nux_f32_t
nux_sin (nux_f32_t x)
{
#ifdef NUX_STDLIB
    return sinf(x);
#endif
}
nux_f32_t
nux_tan (nux_f32_t x)
{
#ifdef NUX_STDLIB
    return tanf(x);
#endif
}
nux_f32_t
nux_exp (nux_f32_t x)
{
#ifdef NUX_STDLIB
    return expf(x);
#endif
}

nux_b2i_t
nux_b2i (nux_v2i_t min, nux_v2i_t max)
{
    nux_b2i_t ret;
    ret.min = min;
    ret.max = max;
    return ret;
}
nux_b2i_t
nux_b2i_xywh (nux_i32_t x, nux_i32_t y, nux_u32_t w, nux_u32_t h)
{
    nux_b2i_t ret;
    ret.min = nux_v2i(x, y);
    ret.max = nux_v2i(x + w - 1, y + h - 1);
    return ret;
}
nux_b2i_t
nux_b2i_translate (nux_b2i_t b, nux_v2i_t t)
{
    return nux_b2i(nux_v2i_add(b.min, t), nux_v2i_add(b.max, t));
}
nux_b2i_t
nux_b2i_moveto (nux_b2i_t b, nux_v2i_t p)
{
    return nux_b2i_translate(b, nux_v2i_sub(p, b.min));
}

#define NUX_B3_IMPL(name, type)                                         \
    nux_##name##_t nux_##name(type min, type max)                       \
    {                                                                   \
        NUX_ASSERT(max.x >= min.x && max.y >= min.y && max.z >= min.z); \
        nux_##name##_t b;                                               \
        b.min = min;                                                    \
        b.max = max;                                                    \
        return b;                                                       \
    }

NUX_B3_IMPL(b3, nux_v3_t);
NUX_B3_IMPL(b3i, nux_v3i_t);
