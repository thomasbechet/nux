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
            nux_point(env, x, y, color);
        }
    }
}
void
nux_point (nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t color)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return);
    env->inst->memory[NUX_MAP_FRAMEBUFFER + y * NUX_SCREEN_WIDTH + x] = color;
}
void
nux_line (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u8_t  color)
{
    nu_v2i_t  v0    = nu_v2i(x0, y0);
    nu_v2i_t  v1    = nu_v2i(x1, y1);
    nu_bool_t steep = NU_FALSE;
    if (NU_ABS(v0.x - v1.x) < NU_ABS(y0 - y1))
    {
        NU_SWAP(v0, v1, nu_v2i_t);
        steep = NU_TRUE;
    }
    if (v0.x > v1.x)
    {
        NU_SWAP(v0, v1, nu_v2i_t);
    }
    nux_i32_t dx      = v1.x - v0.x;
    nux_i32_t dy      = v1.y - v0.y;
    nux_i32_t derror2 = NU_ABS(dy) * 2;
    nux_i32_t error2  = 0;
    nux_i32_t y       = v0.y;
    for (nux_i32_t x = v0.x; x <= v1.x; x++)
    {
        if (steep)
        {
            nux_point(env, y, x, color);
        }
        else
        {
            nux_point(env, x, y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (v1.y > v0.y ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}
static inline void
fill_bottom_triangle (
    nux_env_t env, nu_v2i_t v0, nu_v2i_t v1, nu_v2i_t v2, nux_u8_t c)
{
    nux_f32_t invslope1 = (nux_f32_t)(v1.x - v0.x) / (v1.y - v0.y);
    nux_f32_t invslope2 = (nux_f32_t)(v2.x - v0.x) / (v2.y - v0.y);

    nux_f32_t curx1 = v0.x;
    nux_f32_t curx2 = v0.x;

    for (nux_i32_t y = v0.y; y <= v1.y; ++y)
    {
        nux_line(env, curx1, y, curx2, y, c);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}
static inline void
fill_top_triangle (
    nux_env_t env, nu_v2i_t v0, nu_v2i_t v1, nu_v2i_t v2, nux_u8_t c)
{
    nux_f32_t invslope1 = (nux_f32_t)(v2.x - v0.x) / (v2.y - v0.y);
    nux_f32_t invslope2 = (nux_f32_t)(v2.x - v1.x) / (v2.y - v1.y);

    nux_f32_t curx1 = v2.x;
    nux_f32_t curx2 = v2.x;

    for (nux_i32_t y = v2.y; y > v0.y; y--)
    {
        nux_line(env, curx1, y, curx2, y, c);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}
void
nux_triangle (nux_env_t env,
              nux_i32_t x0,
              nux_i32_t y0,
              nux_i32_t x1,
              nux_i32_t y1,
              nux_i32_t x2,
              nux_i32_t y2,
              nux_u8_t  c)
{
    nu_v2i_t v0 = nu_v2i(x0, y0);
    nu_v2i_t v1 = nu_v2i(x1, y1);
    nu_v2i_t v2 = nu_v2i(x2, y2);
    if (y0 == y1 && y0 == y2)
    {
        return;
    }
    if (y0 > y1)
    {
        NU_SWAP(v0, v1, nu_v2i_t);
    }
    if (y0 > y2)
    {
        NU_SWAP(v0, v2, nu_v2i_t);
    }
    if (y1 > y2)
    {
        NU_SWAP(v1, v2, nu_v2i_t);
    }

    if (v1.y == v2.y)
    {
        fill_bottom_triangle(env, v0, v1, v2, c);
    }
    else if (v0.y == v1.y)
    {
        fill_top_triangle(env, v0, v1, v2, c);
    }
    else
    {
        nu_v2i_t v3 = nu_v2i(
            (nux_i32_t)(v0.x
                        + ((nux_f32_t)(v1.y - v0.y) / (nux_f32_t)(v2.y - v0.y))
                              * (v2.x - v0.x)),
            v1.y);
        fill_bottom_triangle(env, v0, v1, v3, c);
        fill_top_triangle(env, v1, v3, v2, c);
    }
}
