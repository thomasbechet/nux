#ifndef NU_MATH_H
#define NU_MATH_H

#include "platform.h"
#include "types.h"

#define NU_V2_FMT     "%lf %lf"
#define NU_V2_ARGS(v) (v)->x, (v)->y
#define NU_V3_FMT     "%lf %lf %lf"
#define NU_V3_ARGS(v) (v)->x, (v)->y, (v)->z
#define NU_V4_FMT     "%lf %lf %lf %lf"
#define NU_V4_ARGS(v) (v)->x, (v)->y, (v)->z, (v)->w
#define NU_Q4_FMT     NU_V4_FMT
#define NU_Q4_ARGS(q) NU_V4_ARGS(q)
#define NU_M4_FMT     NU_V4_FMT NU_V4_FMT NU_V4_FMT NU_V4_FMT
#define NU_M4_ARGS(m)                           \
    NU_V4_ARGS((nu_v4_t *)((m)->data + 0)),     \
        NU_V4_ARGS((nu_v4_t *)((m)->data + 4)), \
        NU_V4_ARGS((nu_v4_t *)((m)->data + 8)), \
        NU_V4_ARGS((nu_v4_t *)((m)->data + 12))

typedef union
{
    struct
    {
        nu_u8_t r;
        nu_u8_t g;
        nu_u8_t b;
        nu_u8_t a;
    };
    nu_u32_t rgba;
} nu_color_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
    };
    nu_f32_t data[NU_V2_SIZE];
} nu_v2_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
        nu_f32_t z;
    };
    nu_f32_t data[NU_V3_SIZE];
} nu_v3_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
        nu_f32_t z;
        nu_f32_t w;
    };
    nu_f32_t data[NU_V4_SIZE];
} nu_v4_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
    };
    nu_u32_t data[NU_V2_SIZE];
} nu_v2u_t;

typedef union
{
    struct
    {
        nu_i32_t x;
        nu_i32_t y;
    };
    nu_i32_t data[NU_V2_SIZE];
} nu_v2i_t;

typedef union
{
    struct
    {
        nu_i32_t x;
        nu_i32_t y;
        nu_i32_t z;
    };
    nu_i32_t data[NU_V3_SIZE];
} nu_v3i_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
    };
    nu_u32_t data[NU_V3_SIZE];
} nu_v3u_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
        nu_u32_t w;
    };
    nu_u32_t data[NU_V4_SIZE];
} nu_v4u_t;

typedef union
{
    struct
    {
        nu_f32_t x;
        nu_f32_t y;
        nu_f32_t z;
        nu_f32_t w;
    };
    nu_f32_t data[NU_Q4_SIZE];
} nu_q4_t;

typedef union
{

    struct
    {
        nu_f32_t x1;
        nu_f32_t x2;
        nu_f32_t x3;
        nu_f32_t y1;
        nu_f32_t y2;
        nu_f32_t y3;
        nu_f32_t z1;
        nu_f32_t z2;
        nu_f32_t z3;
    };
    nu_f32_t data[NU_M3_SIZE];
} nu_m3_t;

typedef union
{
    struct
    {
        nu_f32_t x1;
        nu_f32_t x2;
        nu_f32_t x3;
        nu_f32_t x4;
        nu_f32_t y1;
        nu_f32_t y2;
        nu_f32_t y3;
        nu_f32_t y4;
        nu_f32_t z1;
        nu_f32_t z2;
        nu_f32_t z3;
        nu_f32_t z4;
        nu_f32_t w1;
        nu_f32_t w2;
        nu_f32_t w3;
        nu_f32_t w4;
    };
    nu_f32_t data[NU_M4_SIZE];
} nu_m4_t;

typedef struct
{
    nu_v2i_t min;
    nu_v2i_t max;
} nu_b2i_t;

typedef struct
{
    nu_v3i_t min;
    nu_v3i_t max;
} nu_b3i_t;

typedef struct
{
    nu_v3_t min;
    nu_v3_t max;
} nu_b3_t;

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API nu_f32_t  nu_fabs(nu_f32_t f);
NU_API nu_f32_t  nu_floor(nu_f32_t f);
NU_API nu_f32_t  nu_radian(nu_f32_t d);
NU_API nu_f32_t  nu_sqrt(nu_f32_t x);
NU_API nu_f32_t  nu_pow(nu_f32_t b, nu_f32_t e);
NU_API nu_f32_t  nu_cos(nu_f32_t x);
NU_API nu_f32_t  nu_sin(nu_f32_t x);
NU_API nu_f32_t  nu_tan(nu_f32_t x);
NU_API nu_f32_t  nu_exp(nu_f32_t x);

NU_API nu_v2_t nu_v2(nu_f32_t x, nu_f32_t y);
NU_API nu_v2_t nu_v2_v2u(nu_v2u_t v);
NU_API nu_v2_t nu_v2_add(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_sub(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_mul(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_muls(nu_v2_t a, nu_f32_t s);
NU_API nu_v2_t nu_v2_div(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_divs(nu_v2_t a, nu_f32_t s);
NU_API nu_v2_t nu_v2_floor(nu_v2_t a);

NU_API nu_v3_t  nu_v3(nu_f32_t x, nu_f32_t y, nu_f32_t z);
NU_API nu_v3_t  nu_v3s(nu_f32_t s);
NU_API nu_v3_t  nu_v3_add(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_sub(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_mul(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_muls(nu_v3_t a, nu_f32_t s);
NU_API nu_v3_t  nu_v3_div(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_divs(nu_v3_t a, nu_f32_t s);
NU_API nu_f32_t nu_v3_norm(nu_v3_t a);
NU_API nu_v3_t  nu_v3_normalize(nu_v3_t a);
NU_API nu_v3_t  nu_v3_cross(nu_v3_t a, nu_v3_t b);
NU_API nu_f32_t nu_v3_dot(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_min(nu_v3_t a, nu_v3_t b);
NU_API nu_v3_t  nu_v3_max(nu_v3_t a, nu_v3_t b);

NU_API nu_v4_t  nu_v4(nu_f32_t x, nu_f32_t y, nu_f32_t z, nu_f32_t w);
NU_API nu_f32_t nu_v4_dot(nu_v4_t a, nu_v4_t b);
NU_API nu_f32_t nu_v4_norm(nu_v4_t v);
NU_API nu_v2_t  nu_v4_xy(nu_v4_t v);
NU_API nu_v2_t  nu_v4_zw(nu_v4_t v);

NU_API nu_v2i_t nu_v2i(nu_i32_t x, nu_i32_t y);
NU_API nu_v2i_t nu_v2i_add(nu_v2i_t a, nu_v2i_t b);
NU_API nu_v2i_t nu_v2i_sub(nu_v2i_t a, nu_v2i_t b);
NU_API nu_v2i_t nu_v2i_min(nu_v2i_t a, nu_v2i_t b);
NU_API nu_v2i_t nu_v2i_max(nu_v2i_t a, nu_v2i_t b);

NU_API nu_v2u_t nu_v2u(nu_u32_t x, nu_u32_t y);
NU_API nu_v2u_t nu_v2u_min(nu_v2u_t a, nu_v2u_t b);

NU_API nu_v3u_t nu_v3u(nu_u32_t x, nu_u32_t y, nu_u32_t z);
NU_API nu_v3u_t nu_v3u_v2u(nu_v2u_t v, nu_u32_t z);
NU_API nu_v2u_t nu_v3u_xy(nu_v3u_t v);

NU_API nu_v4u_t nu_v4u(nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w);

NU_API nu_q4_t  nu_q4(nu_f32_t x, nu_f32_t y, nu_f32_t z, nu_f32_t w);
NU_API nu_q4_t  nu_q4_identity(void);
NU_API nu_v4_t  nu_q4_vec4(nu_q4_t a);
NU_API nu_f32_t nu_q4_norm(nu_q4_t a);
NU_API nu_q4_t  nu_q4_axis(nu_v3_t axis, nu_f32_t angle);
NU_API nu_q4_t  nu_q4_mul(nu_q4_t a, nu_q4_t b);
NU_API nu_v3_t  nu_q4_mulv3(nu_q4_t a, nu_v3_t v);
NU_API nu_q4_t  nu_q4_mul_axis(nu_q4_t q, nu_v3_t axis, nu_f32_t angle);
NU_API nu_m3_t  nu_q4_mat3(nu_q4_t q);
NU_API nu_m4_t  nu_q4_mat4(nu_q4_t q);
NU_API nu_m4_t  nu_q4_mulm4(nu_q4_t a, nu_m4_t m);

NU_API nu_m3_t nu_m3_zero(void);
NU_API nu_m3_t nu_m3_identity(void);
NU_API nu_m3_t nu_m3_translate(nu_v2_t v);
NU_API nu_m3_t nu_m3_scale(nu_v2_t v);
NU_API nu_m3_t nu_m3_mul(nu_m3_t a, nu_m3_t b);

NU_API nu_m4_t nu_m4(const nu_f32_t *p);
NU_API nu_m4_t nu_m4_zero(void);
NU_API nu_m4_t nu_m4_identity(void);
NU_API nu_m4_t nu_m4_translate(nu_v3_t v);
NU_API nu_m4_t nu_m4_scale(nu_v3_t v);
NU_API nu_m4_t nu_m4_rotate_y(nu_f32_t angle);
NU_API nu_m4_t nu_m4_mul(nu_m4_t a, nu_m4_t b);
NU_API nu_v4_t nu_m4_mulv(nu_m4_t a, nu_v4_t v);
NU_API nu_v3_t nu_m4_mulv3(nu_m4_t a, nu_v3_t v);

NU_API nu_b2i_t  nu_b2i(nu_v2i_t min, nu_v2i_t max);
NU_API nu_b2i_t  nu_b2i_xywh(nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h);
NU_API nu_v2u_t  nu_b2i_size(nu_b2i_t b);
NU_API nu_b2i_t  nu_b2i_resize(nu_b2i_t b, nu_v2u_t size);
NU_API nu_v2i_t  nu_b2i_tr(nu_b2i_t b);
NU_API nu_v2i_t  nu_b2i_bl(nu_b2i_t b);
NU_API nu_b2i_t  nu_b2i_translate(nu_b2i_t b, nu_v2i_t t);
NU_API nu_b2i_t  nu_b2i_moveto(nu_b2i_t b, nu_v2i_t p);
NU_API nu_b2i_t  nu_b2i_union(nu_b2i_t a, nu_b2i_t b);
NU_API nu_bool_t nu_b2i_contains(nu_b2i_t b, nu_v2_t p);
NU_API nu_bool_t nu_b2i_containsi(nu_b2i_t b, nu_v2i_t p);
NU_API nu_v2_t   nu_b2i_normalize(nu_b2i_t b, nu_v2_t p);

NU_API nu_b3_t   nu_b3(nu_v3_t p, nu_v3_t s);
NU_API nu_v3_t   nu_b3_center(nu_b3_t b);
NU_API nu_bool_t nu_b3_contains(nu_b3_t b, nu_v3_t p);

NU_API nu_m4_t nu_perspective(nu_f32_t fov,
                              nu_f32_t aspect_ratio,
                              nu_f32_t z_near,
                              nu_f32_t z_far);
NU_API nu_m4_t nu_ortho(nu_f32_t left,
                        nu_f32_t right,
                        nu_f32_t bottom,
                        nu_f32_t top,
                        nu_f32_t near,
                        nu_f32_t far);
NU_API nu_m4_t nu_lookat(nu_v3_t eye, nu_v3_t center, nu_v3_t up);

NU_API nu_v3_t nu_axis3d(nu_f32_t  pos_x,
                         nu_f32_t  neg_x,
                         nu_f32_t  pos_y,
                         nu_f32_t  neg_y,
                         nu_f32_t  pos_z,
                         nu_f32_t  neg_z,
                         nu_bool_t normalize);

NU_API nu_v3_t nu_triangle_normal(nu_v3_t p0, nu_v3_t p1, nu_v3_t p2);

NU_API nu_color_t nu_color(nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a);
NU_API nu_v4_t    nu_color_to_vec4(nu_color_t c);
NU_API nu_color_t nu_color_from_vec4(nu_v4_t v);
NU_API nu_color_t nu_color_to_srgb(nu_color_t c);
NU_API nu_color_t nu_color_to_linear(nu_color_t c);

NU_API nu_u32_t nu_pcg_u32(nu_u64_t *state, nu_u64_t incr);
NU_API nu_f32_t nu_pcg_f32(nu_u64_t *state, nu_u64_t incr);

#endif
