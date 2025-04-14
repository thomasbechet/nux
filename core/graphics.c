#include "internal.h"

void
nux_clear (nux_env_t env, nux_u32_t color)
{
    nux_fill(env, 0, 0, NUX_SCREEN_WIDTH - 1, NUX_SCREEN_HEIGHT - 1, color);
}
void
nux_fill (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u32_t color)
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
nux_point (nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u32_t color)
{
    env->inst->memory[(y * NUX_SCREEN_WIDTH + x) * 3 + 0]
        = (color >> 24) & 0xFF;
    env->inst->memory[(y * NUX_SCREEN_WIDTH + x) * 3 + 1]
        = (color >> 16) & 0xFF;
    env->inst->memory[(y * NUX_SCREEN_WIDTH + x) * 3 + 2] = (color >> 8) & 0xFF;
}
void
nux_line (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u32_t color)
{
    nu_bool_t steep = NU_FALSE;
    if (NU_ABS(x0 - x1) < NU_ABS(y0 - y1))
    {
        NU_SWAP(x0, y0, nux_i32_t);
        NU_SWAP(x1, y1, nux_i32_t);
        steep = NU_TRUE;
    }
    if (x0 > x1)
    {
        NU_SWAP(x0, x1, nux_i32_t);
        NU_SWAP(y0, y1, nux_i32_t);
    }
    nux_i32_t dx      = x1 - x0;
    nux_i32_t dy      = y1 - y0;
    nux_i32_t derror2 = NU_ABS(dy) * 2;
    nux_i32_t error2  = 0;
    nux_i32_t y       = y0;
    for (nux_i32_t x = x0; x <= x1; x++)
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
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
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
              nux_u32_t color)
{
    if (y0 == y1 && y0 == y2)
    {
        return; // I dont care about degenerate triangles
    }
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if (y0 > y1)
    {
        NU_SWAP(x0, x1, nux_i32_t);
        NU_SWAP(y0, y1, nux_i32_t);
    }
    if (y0 > y2)
    {
        NU_SWAP(x0, x2, nux_i32_t);
        NU_SWAP(y0, y2, nux_i32_t);
    }
    if (y1 > y2)
    {
        NU_SWAP(x1, x2, nux_i32_t);
        NU_SWAP(y1, y2, nux_i32_t);
    }
    nux_i32_t total_height = y2 - y0;
    for (nux_i32_t i = 0; i < total_height; ++i)
    {
        nu_bool_t second_half    = i > y1 - y0 || y1 == y0;
        nux_i32_t segment_height = second_half ? y2 - y1 : y1 - y0;
        nux_f32_t alpha          = (nux_f32_t)i / total_height;
        nux_f32_t beta           = (nux_f32_t)(i - (second_half ? y1 - y0 : 0))
                         / segment_height; // be careful: with above conditions
                                           // no division by zero here
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x > B.x)
        {
            std::swap(A, B);
        }
        for (int j = A.x; j <= B.x; j++)
        {
            image.set(j,
                      y0 + i,
                      color); // attention, due to int casts y0+i != A.y
        }
    }
}
