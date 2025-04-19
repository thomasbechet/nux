#include "internal.h"

// The '(x != 0 && y != 0)' test in the last line of this function
// may be omitted for a performance benefit if the radius of the
// circle is known to be non-zero.
void
plot4points (nux_env_t env, int cx, int cy, int x, int y, nux_u8_t c)
{
    nux_point(env, cx + x, cy + y, c);
    if (x != 0)
    {
        nux_point(env, cx - x, cy + y, c);
    }
    if (y != 0)
    {
        nux_point(env, cx + x, cy - y, c);
    }
    if (x != 0 && y != 0)
    {
        nux_point(env, cx - x, cy - y, c);
    }
}
void
plot8points (nux_env_t env, int cx, int cy, int x, int y, nux_u8_t c)
{
    plot4points(env, cx, cy, x, y, c);
    if (x != y)
    {
        plot4points(env, cx, cy, y, x, c);
    }
}
void
circle (nux_env_t env, int cx, int cy, int radius, nux_u8_t c)
{
    int error = -radius;
    int x     = radius;
    int y     = 0;

    // The following while loop may altered to 'while (x > y)' for a
    // performance benefit, as long as a call to 'plot4points' follows
    // the body of the loop. This allows for the elimination of the
    // '(x != y') test in 'plot8points', providing a further benefit.
    //
    // For the sake of clarity, this is not shown here.
    while (x >= y)
    {
        plot8points(env, cx, cy, x, y, c);
        error += y;
        ++y;
        error += y;
        // The following test may be implemented in assembly language in
        // most machines by testing the carry flag after adding 'y' to
        // the value of 'error' in the previous step, since 'error'
        // nominally has a negative value.
        if (error >= 0)
        {
            --x;
            error -= x;
            error -= x;
        }
    }
}
void
plot_circle (nux_env_t env, int xm, int ym, int r, nux_u8_t c)
{
    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do
    {
        nux_point(env, xm - x, ym + y, c); /*   I. Quadrant */
        nux_point(env, xm - y, ym - x, c); /*  II. Quadrant */
        nux_point(env, xm + x, ym - y, c); /* III. Quadrant */
        nux_point(env, xm + y, ym + x, c); /*  IV. Quadrant */
        r = err;
        if (r > x)
        {
            err += ++x * 2 + 1; /* e_xy+e_x > 0 */
        }
        if (r <= y)
        {
            err += ++y * 2 + 1; /* e_xy+e_y < 0 */
        }
    } while (x < 0);
}
void
raster_circle (nux_env_t env, int x0, int y0, int radius, nux_c8_t c)
{
    int f     = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x     = 0;
    int y     = radius;

    nux_point(env, x0, y0 + radius, c);
    nux_point(env, x0, y0 - radius, c);
    nux_point(env, x0 + radius, y0, c);
    nux_point(env, x0 - radius, y0, c);
    while (x < y)
    {
        // ddF_x == 2 * x + 1;
        // ddF_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        nux_point(env, x0 + x, y0 + y, c);
        nux_point(env, x0 - x, y0 + y, c);
        nux_point(env, x0 + x, y0 - y, c);
        nux_point(env, x0 - x, y0 - y, c);
        nux_point(env, x0 + y, y0 + x, c);
        nux_point(env, x0 - y, y0 + x, c);
        nux_point(env, x0 + y, y0 - x, c);
        nux_point(env, x0 - y, y0 - x, c);
    }
}

void
loop_init (nux_env_t env)
{
    nux_palset(env, 0, 0x0B3954);
    nux_palset(env, 1, 0xBFD7EA);
    nux_palset(env, 2, 0xFF6663);
    nux_palset(env, 3, 0xFF0000);
    nux_palset(env, 4, 0x00FF00);
    nux_palset(env, 5, 0x0000FF);
    nux_palset(env, 6, 0);
}
void
loop_update (nux_env_t env)
{
    nux_clear(env, 6);
    nu_v2i_t v0 = nu_v2i(50, 50);
    nu_v2i_t v1 = nu_v2i(100 + nu_cos(nux_global_time(env) * 0.5) * 50,
                         70 + nu_sin(nux_global_time(env) * 0.5) * 50);
    nu_v2i_t v2 = nu_v2i(80, 200);
    nux_triangle(env, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, 1);
    nux_line(env, 150, 150, 300, 20, 2);
    for (int i = 0; i < 256; ++i)
    {
        const nu_u32_t size = 8;
        nux_fill(env, i * size, 0, i * size + size - 1, size - 1, i);
    }
    // circle(env, 100, 100, 50, 3);
    // plot_circle(env, 100, 100, 50, 3);
    raster_circle(env, 100, 100, 50, 3);
}
