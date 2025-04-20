#include "internal.h"

void
nux_fill (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u8_t  color)
{
    nux_i32_t minx = NU_MIN(x0, x1);
    nux_i32_t miny = NU_MIN(y0, y1);
    nux_i32_t maxx = NU_MAX(x0, x1);
    nux_i32_t maxy = NU_MAX(y0, y1);
    for (nux_i32_t y = miny; y <= maxy; ++y)
    {
        for (nux_i32_t x = minx; x <= maxx; ++x)
        {
            nux_pset(env, x, y, color);
        }
    }
}
void
nux_pset (nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t color)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return);
    env->inst->memory[NUX_MAP_SCREEN + y * NUX_SCREEN_WIDTH + x] = color;
}
nux_u8_t
nux_pget (nux_env_t env, nux_i32_t x, nux_i32_t y)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return 0);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return 0);
    return env->inst->memory[NUX_MAP_SCREEN + y * NUX_SCREEN_WIDTH + x];
}
void
nux_line (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u8_t  c)
{
    nu_i32_t dx  = NU_ABS(x1 - x0);
    nu_i32_t sx  = x0 < x1 ? 1 : -1;
    nu_i32_t dy  = -NU_ABS(y1 - y0);
    nu_i32_t sy  = y0 < y1 ? 1 : -1;
    nu_i32_t err = dx + dy;
    nu_i32_t e2;
    for (;;)
    {
        nux_pset(env, x0, y0, c);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
static inline void
fill_triangle (nux_env_t env,
               nux_i32_t x0,
               nux_i32_t y0,
               nux_i32_t x1,
               nux_i32_t y1,
               nux_i32_t x2,
               nux_i32_t y2,
               nux_u8_t  c)
{
    // v0.y <= v1.y <= v2.y
    nu_v2i_t v0 = nu_v2i(x0, y0);
    nu_v2i_t v1 = nu_v2i(x1, y1);
    nu_v2i_t v2 = nu_v2i(x2, y2);
    if (y0 == y1 && y0 == y2)
    {
        nux_line(env,
                 NU_MIN(x0, NU_MIN(x1, x2)),
                 y0,
                 NU_MAX(x0, NU_MAX(x1, x2)),
                 y0,
                 c);
        return;
    }
    if (v0.y > v1.y)
    {
        NU_SWAP(v0, v1, nu_v2i_t);
    }
    if (v0.y > v2.y)
    {
        NU_SWAP(v0, v2, nu_v2i_t);
    }
    if (v1.y > v2.y)
    {
        NU_SWAP(v1, v2, nu_v2i_t);
    }

    // 3 bresenham iterations :
    //  v0 -> v1 (bresenham0) (check transition to v2)
    //  v1 -> v2 (bresenham1)
    //  v0 -> v2 (bresenham1) (longest iteration)

    nu_i32_t  dx0  = NU_ABS(v1.x - v0.x);
    nu_i32_t  dx1  = NU_ABS(v2.x - v0.x);
    nu_i32_t  sx0  = v0.x < v1.x ? 1 : -1;
    nu_i32_t  sx1  = v0.x < v2.x ? 1 : -1;
    nu_i32_t  dy0  = -NU_ABS(v1.y - v0.y);
    nu_i32_t  dy1  = -NU_ABS(v2.y - v0.y);
    nu_i32_t  sy0  = v0.y < v1.y ? 1 : -1;
    nu_i32_t  sy1  = v0.y < v2.y ? 1 : -1;
    nu_i32_t  err0 = dx0 + dy0;
    nu_i32_t  err1 = dx1 + dy1;
    nu_i32_t  e20, e21;
    nu_i32_t  curx0 = v0.x;
    nu_i32_t  curx1 = v0.x;
    nu_i32_t  cury0 = v0.y;
    nu_i32_t  cury1 = v0.y;
    nu_bool_t lower = NU_FALSE;

bresenham0:
    for (;;)
    {
        if (curx0 == v1.x && cury0 == v1.y && !lower)
        {
            // End of first iteration, transition to v1 -> v2
            dx0   = NU_ABS(v2.x - v1.x);
            sx0   = v1.x < v2.x ? 1 : -1;
            dy0   = -NU_ABS(v2.y - v1.y);
            sy0   = v1.y < v2.y ? 1 : -1;
            err0  = dx0 + dy0;
            curx0 = v1.x;
            cury0 = v1.y;
            lower = NU_TRUE;
        }
        e20 = 2 * err0;
        if (e20 >= dy0)
        {
            err0 += dy0;
            curx0 += sx0;
        }
        if (e20 <= dx0)
        {
            err0 += dx0;
            cury0 += sy0;
            goto bresenham1;
        }
    }

bresenham1:
    for (;;)
    {
        e21 = 2 * err1;
        if (e21 >= dy1)
        {
            err1 += dy1;
            curx1 += sx1;
        }
        if (e21 <= dx1)
        {
            err1 += dx1;
            cury1 += sy1;
            goto processline;
        }
    }

processline:
    for (nu_i32_t x = NU_MIN(curx0, curx1); x < NU_MAX(curx0, curx1); ++x)
    {
        nux_pset(env, x, cury1, c);
    }
    if (cury1 == v2.y)
    {
        return;
    }
    goto bresenham0;
}
// static inline void
// fill_bottom_triangle (
//     nux_env_t env, nu_v2i_t v0, nu_v2i_t v1, nu_v2i_t v2, nux_u8_t c)
// {
//     nux_f32_t invslope1 = (nux_f32_t)(v1.x - v0.x) / (v1.y - v0.y);
//     nux_f32_t invslope2 = (nux_f32_t)(v2.x - v0.x) / (v2.y - v0.y);
//
//     nux_f32_t curx1 = v0.x;
//     nux_f32_t curx2 = v0.x;
//
//     for (nux_i32_t y = v0.y; y <= v1.y; ++y)
//     {
//         nux_line(env, curx1, y, curx2, y, c);
//         curx1 += invslope1;
//         curx2 += invslope2;
//     }
// }
// static inline void
// fill_top_triangle (
//     nux_env_t env, nu_v2i_t v0, nu_v2i_t v1, nu_v2i_t v2, nux_u8_t c)
// {
//     nux_f32_t invslope1 = (nux_f32_t)(v2.x - v0.x) / (v2.y - v0.y);
//     nux_f32_t invslope2 = (nux_f32_t)(v2.x - v1.x) / (v2.y - v1.y);
//
//     nux_f32_t curx1 = v2.x;
//     nux_f32_t curx2 = v2.x;
//
//     for (nux_i32_t y = v2.y; y > v0.y; y--)
//     {
//         nux_line(env, curx1, y, curx2, y, c);
//         curx1 -= invslope1;
//         curx2 -= invslope2;
//     }
// }
void
nux_filltri (nux_env_t env,
             nux_i32_t x0,
             nux_i32_t y0,
             nux_i32_t x1,
             nux_i32_t y1,
             nux_i32_t x2,
             nux_i32_t y2,
             nux_u8_t  c)
{
    fill_triangle(env, x0, y0, x1, y1, x2, y2, c);
    return;

    // nu_v2i_t v0 = nu_v2i(x0, y0);
    // nu_v2i_t v1 = nu_v2i(x1, y1);
    // nu_v2i_t v2 = nu_v2i(x2, y2);
    // if (y0 == y1 && y0 == y2)
    // {
    //     return;
    // }
    // if (y0 > y1)
    // {
    //     NU_SWAP(v0, v1, nu_v2i_t);
    // }
    // if (y0 > y2)
    // {
    //     NU_SWAP(v0, v2, nu_v2i_t);
    // }
    // if (y1 > y2)
    // {
    //     NU_SWAP(v1, v2, nu_v2i_t);
    // }
    //
    // if (v1.y == v2.y)
    // {
    //     fill_bottom_triangle(env, v0, v1, v2, c);
    // }
    // else if (v0.y == v1.y)
    // {
    //     fill_top_triangle(env, v0, v1, v2, c);
    // }
    // else
    // {
    //     nu_v2i_t v3 = nu_v2i(
    //         (nux_i32_t)(v0.x
    //                     + ((nux_f32_t)(v1.y - v0.y) / (nux_f32_t)(v2.y -
    //                     v0.y))
    //                           * (nux_f32_t)(v2.x - v0.x)),
    //         v1.y);
    //     fill_bottom_triangle(env, v0, v1, v3, c);
    //     fill_top_triangle(env, v1, v3, v2, c);
    // }
}
