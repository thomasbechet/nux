#include "common.h"

#define nux_v2_impl(name, type, opadd, opsub, opmul, opdiv, opmin, opmax) \
    nux_##name##_t nux_##name(type x, type y)                             \
    {                                                                     \
        nux_##name##_t v;                                                 \
        v.x = x;                                                          \
        v.y = y;                                                          \
        return v;                                                         \
    }                                                                     \
    nux_##name##_t nux_##name##s(type x)                                  \
    {                                                                     \
        return nux_##name(x, x);                                          \
    }                                                                     \
    nux_##name##_t nux_##name##_zero(void)                                \
    {                                                                     \
        return nux_##name##s(0);                                          \
    }                                                                     \
    nux_##name##_t nux_##name##_one(void)                                 \
    {                                                                     \
        return nux_##name##s(1);                                          \
    }                                                                     \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b)   \
    {                                                                     \
        return nux_##name(opadd(a.x, b.x), opadd(a.y, b.y));              \
    }                                                                     \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b)   \
    {                                                                     \
        return nux_##name(opsub(a.x, b.x), opsub(a.y, b.y));              \
    }                                                                     \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b)   \
    {                                                                     \
        return nux_##name(opmul(a.x, b.x), opmul(a.y, b.y));              \
    }                                                                     \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b)   \
    {                                                                     \
        return nux_##name(opdiv(a.x, b.x), opdiv(a.y, b.y));              \
    }                                                                     \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b)            \
    {                                                                     \
        return nux_##name##_add(a, nux_##name##s(b));                     \
    }                                                                     \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b)            \
    {                                                                     \
        return nux_##name##_sub(a, nux_##name##s(b));                     \
    }                                                                     \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b)            \
    {                                                                     \
        return nux_##name##_mul(a, nux_##name##s(b));                     \
    }                                                                     \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b)            \
    {                                                                     \
        return nux_##name##_div(a, nux_##name##s(b));                     \
    }                                                                     \
    nux_##name##_t nux_##name##_min(nux_##name##_t a, nux_##name##_t b)   \
    {                                                                     \
        return nux_##name(opmin(a.x, b.x), opmin(a.y, b.y));              \
    }                                                                     \
    nux_##name##_t nux_##name##_max(nux_##name##_t a, nux_##name##_t b)   \
    {                                                                     \
        return nux_##name(opmax(a.x, b.x), opmax(a.y, b.y));              \
    }                                                                     \
    type nux_##name##_dot(nux_##name##_t a, nux_##name##_t b)             \
    {                                                                     \
        return a.x * b.x + a.y * b.y;                                     \
    }                                                                     \
    nux_f32_t nux_##name##_norm(nux_##name##_t a)                         \
    {                                                                     \
        return nux_sqrt(nux_##name##_dot(a, a));                          \
    }                                                                     \
    nux_f32_t nux_##name##_dist(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                     \
        return nux_##name##_norm(nux_##name##_sub(a, b));                 \
    }                                                                     \
    type nux_##name##_dist2(nux_##name##_t a, nux_##name##_t b)           \
    {                                                                     \
        nux_##name##_t sub = nux_##name##_sub(a, b);                      \
        return nux_##name##_dot(sub, sub);                                \
    }

#define nux_v3_impl(name, type, opadd, opsub, opmul, opdiv, opmin, opmax)     \
    nux_##name##_t nux_##name(type x, type y, type z)                         \
    {                                                                         \
        nux_##name##_t v;                                                     \
        v.x = x;                                                              \
        v.y = y;                                                              \
        v.z = z;                                                              \
        return v;                                                             \
    }                                                                         \
    nux_##name##_t nux_##name##s(type x)                                      \
    {                                                                         \
        return nux_##name(x, x, x);                                           \
    }                                                                         \
    nux_##name##_t nux_##name##_zero(void)                                    \
    {                                                                         \
        return nux_##name##s(0);                                              \
    }                                                                         \
    nux_##name##_t nux_##name##_one(void)                                     \
    {                                                                         \
        return nux_##name##s(1);                                              \
    }                                                                         \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                         \
        return nux_##name(opadd(a.x, b.x), opadd(a.y, b.y), opadd(a.z, b.z)); \
    }                                                                         \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                         \
        return nux_##name(opsub(a.x, b.x), opsub(a.y, b.y), opsub(a.z, b.z)); \
    }                                                                         \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                         \
        return nux_##name(opmul(a.x, b.x), opmul(a.y, b.y), opmul(a.z, b.z)); \
    }                                                                         \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                         \
        return nux_##name(opdiv(a.x, b.x), opdiv(a.y, b.y), opdiv(a.z, b.z)); \
    }                                                                         \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b)                \
    {                                                                         \
        return nux_##name##_add(a, nux_##name##s(b));                         \
    }                                                                         \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b)                \
    {                                                                         \
        return nux_##name##_sub(a, nux_##name##s(b));                         \
    }                                                                         \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b)                \
    {                                                                         \
        return nux_##name##_mul(a, nux_##name##s(b));                         \
    }                                                                         \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b)                \
    {                                                                         \
        return nux_##name##_div(a, nux_##name##s(b));                         \
    }                                                                         \
    nux_##name##_t nux_##name##_inv(nux_##name##_t a)                         \
    {                                                                         \
        nux_assert(a.x != 0 && a.y != 0 && a.z != 0);                         \
        return nux_##name##_div(nux_##name##s(1), a);                         \
    }                                                                         \
    nux_##name##_t nux_##name##_cross(nux_##name##_t a, nux_##name##_t b)     \
    {                                                                         \
        nux_##name##_t v;                                                     \
        v.x = a.y * b.z - a.z * b.y;                                          \
        v.y = a.z * b.x - a.x * b.z;                                          \
        v.z = a.x * b.y - a.y * b.x;                                          \
        return v;                                                             \
    }                                                                         \
    type nux_##name##_dot(nux_##name##_t a, nux_##name##_t b)                 \
    {                                                                         \
        return a.x * b.x + a.y * b.y + a.z * b.z;                             \
    }                                                                         \
    nux_##name##_t nux_##name##_min(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                         \
        return nux_##name(opmin(a.x, b.x), opmin(a.y, b.y), opmin(a.z, b.z)); \
    }                                                                         \
    nux_##name##_t nux_##name##_max(nux_##name##_t a, nux_##name##_t b)       \
    {                                                                         \
        return nux_##name(opmax(a.x, b.x), opmax(a.y, b.y), opmax(a.z, b.z)); \
    }                                                                         \
    nux_f32_t nux_##name##_norm(nux_##name##_t a)                             \
    {                                                                         \
        return nux_sqrt(nux_##name##_dot(a, a));                              \
    }                                                                         \
    nux_f32_t nux_##name##_dist(nux_##name##_t a, nux_##name##_t b)           \
    {                                                                         \
        return nux_##name##_norm(nux_##name##_sub(a, b));                     \
    }                                                                         \
    type nux_##name##_dist2(nux_##name##_t a, nux_##name##_t b)               \
    {                                                                         \
        nux_##name##_t sub = nux_##name##_sub(a, b);                          \
        return nux_##name##_dot(sub, sub);                                    \
    }

#define nux_v4_impl(name, type, opadd, opsub, opmul, opdiv)             \
    nux_##name##_t nux_##name(type x, type y, type z, type w)           \
    {                                                                   \
        nux_##name##_t v;                                               \
        v.x = x;                                                        \
        v.y = y;                                                        \
        v.z = z;                                                        \
        v.w = w;                                                        \
        return v;                                                       \
    }                                                                   \
    nux_##name##_t nux_##name##s(type x)                                \
    {                                                                   \
        return nux_##name(x, x, x, x);                                  \
    }                                                                   \
    nux_##name##_t nux_##name##_zero(void)                              \
    {                                                                   \
        return nux_##name##s(0);                                        \
    }                                                                   \
    nux_##name##_t nux_##name##_one(void)                               \
    {                                                                   \
        return nux_##name##s(1);                                        \
    }                                                                   \
    nux_##name##_t nux_##name##_add(nux_##name##_t a, nux_##name##_t b) \
    {                                                                   \
        return nux_##name(opadd(a.x, b.x),                              \
                          opadd(a.y, b.y),                              \
                          opadd(a.z, b.z),                              \
                          opadd(a.w, b.w));                             \
    }                                                                   \
    nux_##name##_t nux_##name##_sub(nux_##name##_t a, nux_##name##_t b) \
    {                                                                   \
        return nux_##name(opsub(a.x, b.x),                              \
                          opsub(a.y, b.y),                              \
                          opsub(a.z, b.z),                              \
                          opsub(a.w, b.w));                             \
    }                                                                   \
    nux_##name##_t nux_##name##_mul(nux_##name##_t a, nux_##name##_t b) \
    {                                                                   \
        return nux_##name(opmul(a.x, b.x),                              \
                          opmul(a.y, b.y),                              \
                          opmul(a.z, b.z),                              \
                          opmul(a.w, b.w));                             \
    }                                                                   \
    nux_##name##_t nux_##name##_div(nux_##name##_t a, nux_##name##_t b) \
    {                                                                   \
        return nux_##name(opdiv(a.x, b.x),                              \
                          opdiv(a.y, b.y),                              \
                          opdiv(a.z, b.z),                              \
                          opdiv(a.w, b.w));                             \
    }                                                                   \
    nux_##name##_t nux_##name##_adds(nux_##name##_t a, type b)          \
    {                                                                   \
        return nux_##name##_add(a, nux_##name##s(b));                   \
    }                                                                   \
    nux_##name##_t nux_##name##_subs(nux_##name##_t a, type b)          \
    {                                                                   \
        return nux_##name##_sub(a, nux_##name##s(b));                   \
    }                                                                   \
    nux_##name##_t nux_##name##_muls(nux_##name##_t a, type b)          \
    {                                                                   \
        return nux_##name##_mul(a, nux_##name##s(b));                   \
    }                                                                   \
    nux_##name##_t nux_##name##_divs(nux_##name##_t a, type b)          \
    {                                                                   \
        return nux_##name##_div(a, nux_##name##s(b));                   \
    }                                                                   \
    type nux_##name##_dot(nux_##name##_t a, nux_##name##_t b)           \
    {                                                                   \
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;           \
    }                                                                   \
    nux_f32_t nux_##name##_norm(nux_##name##_t a)                       \
    {                                                                   \
        return nux_sqrt(nux_##name##_dot(a, a));                        \
    }                                                                   \
    nux_f32_t nux_##name##_dist(nux_##name##_t a, nux_##name##_t b)     \
    {                                                                   \
        return nux_##name##_norm(nux_##name##_sub(a, b));               \
    }                                                                   \
    type nux_##name##_dist2(nux_##name##_t a, nux_##name##_t b)         \
    {                                                                   \
        nux_##name##_t sub = nux_##name##_sub(a, b);                    \
        return nux_##name##_dot(sub, sub);                              \
    }

#define native_add(a, b) (a + b)
#define native_sub(a, b) (a - b)
#define native_mul(a, b) (a * b)
#define native_div(a, b) (a / b)
#define native_min(a, b) nux_min(a, b)
#define native_max(a, b) nux_max(a, b)

nux_v2_impl(v2i,
            nux_i32_t,
            native_add,
            native_sub,
            native_mul,
            native_div,
            native_min,
            native_max);
nux_v2_impl(v2u,
            nux_u32_t,
            native_add,
            native_sub,
            native_mul,
            native_div,
            native_min,
            native_max);
nux_v2_impl(v2,
            nux_f32_t,
            native_add,
            native_sub,
            native_mul,
            native_div,
            native_min,
            native_max);
nux_v3_impl(v3i,
            nux_i32_t,
            native_add,
            native_sub,
            native_mul,
            native_div,
            native_min,
            native_max);
nux_v3_impl(v3u,
            nux_u32_t,
            native_add,
            native_sub,
            native_mul,
            native_div,
            native_min,
            native_max);
nux_v3_impl(v3,
            nux_f32_t,
            native_add,
            native_sub,
            native_mul,
            native_div,
            native_min,
            native_max);
nux_v4_impl(v4i, nux_i32_t, native_add, native_sub, native_mul, native_div);
nux_v4_impl(v4u, nux_u32_t, native_add, native_sub, native_mul, native_div);
nux_v4_impl(v4, nux_f32_t, native_add, native_sub, native_mul, native_div);

nux_v3_t
nux_v3_normalize (nux_v3_t a)
{
    nux_f32_t norm = nux_v3_norm(a);
    if (norm == 0)
    {
        return nux_v3_zero();
    }
    nux_v3_t ret;
    ret.x = a.x / norm;
    ret.y = a.y / norm;
    ret.z = a.z / norm;
    return ret;
}
nux_v3_t
nux_v3_up (void)
{
    return nux_v3(0, 1, 0);
}
nux_v3_t
nux_v3_down (void)
{
    return nux_v3(0, -1, 0);
}
nux_v3_t
nux_v3_forward (void)
{
    return nux_v3(0, 0, -1);
}
nux_v3_t
nux_v3_backward (void)
{
    return nux_v3(0, 0, 1);
}
nux_v3_t
nux_v3_left (void)
{
    return nux_v3(-1, 0, 0);
}
nux_v3_t
nux_v3_right (void)
{
    return nux_v3(1, 0, 0);
}
