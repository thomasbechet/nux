#include "internal.h"

static struct
{
    nu_u32_t player;

    nu_v3_t  pos;
    nu_v3_t  vel;
    nu_v3_t  acc;
    nu_q4_t  rot;
    nu_f32_t yaw;
    nu_f32_t pitch;

    nu_f32_t fov;
    nu_f32_t speed;
} freecam;

void
init_debug_camera (nu_v3_t pos)
{
    freecam.player = 0;

    freecam.pos   = pos;
    freecam.vel   = NU_V3_ZEROS;
    freecam.acc   = NU_V3_ZEROS;
    freecam.rot   = nu_q4_identity();
    freecam.pitch = 0;
    freecam.yaw   = 0;

    freecam.fov   = 50;
    freecam.speed = 10;
}

nu_m4_t
debug_camera (nux_env_t env, nu_f32_t dt)
{
    nu_v3_t look = nu_v3(nux_axs(env, freecam.player, NUX_AXIS_RIGHTX) * 100,
                         nux_axs(env, freecam.player, NUX_AXIS_RIGHTY) * 100,
                         0);

    nu_v3_t move = nu_v3(nux_axs(env, freecam.player, NUX_AXIS_LEFTX),
                         0,
                         nux_axs(env, freecam.player, NUX_AXIS_LEFTY));
    move.y += nux_btn(env, freecam.player) & NUX_BUTTON_Y ? 1 : 0;
    move.y -= nux_btn(env, freecam.player) & NUX_BUTTON_B ? 1 : 0;
    move = nu_v3_normalize(move);

    // Translation
    nu_v3_t direction = NU_V3_ZEROS;

    direction = nu_v3_add(
        direction, nu_v3_muls(nu_q4_mulv3(freecam.rot, NU_V3_FORWARD), move.z));
    direction = nu_v3_add(
        direction, nu_v3_muls(nu_q4_mulv3(freecam.rot, NU_V3_RIGHT), move.x));
    direction = nu_v3_add(direction, nu_v3_muls(NU_V3_UP, move.y));
    direction = nu_v3_normalize(direction);

    // Rotation
    if (look.x != 0)
    {
        freecam.yaw += look.x * dt;
    }
    if (look.y != 0)
    {
        freecam.pitch -= look.y * dt;
    }
    freecam.pitch = NU_CLAMP(freecam.pitch, -90.0, 90.0);
    freecam.rot   = nu_q4_axis(NU_V3_UP, -nu_radian(freecam.yaw));
    freecam.rot   = nu_q4_mul(freecam.rot,
                            nu_q4_axis(NU_V3_RIGHT, -nu_radian(freecam.pitch)));

    // Compute sum of forces
    const nu_f32_t mass  = 10.0;
    nu_v3_t        force = NU_V3_ZEROS;

    // Apply movement
    if (nu_v3_norm(direction) > 0.001)
    {
        force = nu_v3_add(force, nu_v3_muls(direction, 5));
    }

    // Apply drag
    force = nu_v3_add(force, nu_v3_muls(freecam.vel, -0.5f));

    // Integrate
    freecam.pos     = nu_v3_add(freecam.pos,
                            nu_v3_add(nu_v3_muls(freecam.vel, dt),
                                      nu_v3_muls(freecam.acc, 0.5f * dt * dt)));
    nu_v3_t new_acc = nu_v3_muls(force, mass);
    nu_v3_t new_vel = nu_v3_add(
        freecam.vel, nu_v3_muls(nu_v3_add(freecam.acc, new_acc), 0.5f * dt));
    if (nu_v3_norm(new_vel) < 0.1)
    {
        new_vel = NU_V3_ZEROS;
    }
    freecam.acc = new_acc;
    freecam.vel = new_vel;

    nu_v3_t pos     = freecam.pos;
    nu_v3_t forward = nu_q4_mulv3(freecam.rot, NU_V3_FORWARD);
    nu_v3_t up      = nu_v3_normalize(nu_q4_mulv3(freecam.rot, NU_V3_UP));

    nu_m4_t view = nu_lookat(pos, nu_v3_add(pos, forward), up);
    nu_m4_t proj = nu_perspective(nu_radian(60.0),
                                  (nu_f32_t)NUX_SCREEN_WIDTH
                                      / (nu_f32_t)NUX_SCREEN_HEIGHT,
                                  0.5f,
                                  10000);

    return nu_m4_mul(proj, view);
}

// The '(x != 0 && y != 0)' test in the last line of this function
// may be omitted for a performance benefit if the radius of the
// circle is known to be non-zero.
void
plot4points (nux_env_t env, int cx, int cy, int x, int y, nux_u8_t c)
{
    nux_pset(env, cx + x, cy + y, c);
    if (x != 0)
    {
        nux_pset(env, cx - x, cy + y, c);
    }
    if (y != 0)
    {
        nux_pset(env, cx + x, cy - y, c);
    }
    if (x != 0 && y != 0)
    {
        nux_pset(env, cx - x, cy - y, c);
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
        nux_pset(env, xm - x, ym + y, c); /*   I. Quadrant */
        nux_pset(env, xm - y, ym - x, c); /*  II. Quadrant */
        nux_pset(env, xm + x, ym - y, c); /* III. Quadrant */
        nux_pset(env, xm + y, ym + x, c); /*  IV. Quadrant */
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

    nux_pset(env, x0, y0 + radius, c);
    nux_pset(env, x0, y0 - radius, c);
    nux_pset(env, x0 + radius, y0, c);
    nux_pset(env, x0 - radius, y0, c);
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
        nux_pset(env, x0 + x, y0 + y, c);
        nux_pset(env, x0 - x, y0 + y, c);
        nux_pset(env, x0 + x, y0 - y, c);
        nux_pset(env, x0 - x, y0 - y, c);
        nux_pset(env, x0 + y, y0 + x, c);
        nux_pset(env, x0 - y, y0 + x, c);
        nux_pset(env, x0 + y, y0 - x, c);
        nux_pset(env, x0 - y, y0 - x, c);
    }
}

void
loop_init (nux_env_t env)
{
    nux_pal(env, 0, 0x0B3954);
    nux_pal(env, 1, 0xBFD7EA);
    nux_pal(env, 2, 0xFF6663);
    nux_pal(env, 3, 0xFF0000);
    nux_pal(env, 4, 0x00FF00);
    nux_pal(env, 5, 0x0000FF);
    nux_pal(env, 6, 0);
    nux_pal(env, 7, 0xFFFFFF);
    init_debug_camera(NU_V3_ZEROS);
}
void
loop_update (nux_env_t env)
{
    nux_cls(env, 6);
    nux_clsz(env);
    nu_v2i_t v0 = nu_v2i(50, 50);
    nu_v2i_t v1 = nu_v2i(100 + nu_cos(nux_time(env) * 0.5) * 50,
                         70 + nu_sin(nux_time(env) * 0.5) * 50);
    nu_v2i_t v2 = nu_v2i(80, 200);
    nux_filltri(env, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, 1);
    nux_line(env, 150, 150, 300, 20, 2);
    // circle(env, 100, 100, 50, 3);
    // plot_circle(env, 100, 100, 50, 3);

    // nu_m4_t vp = debug_camera(env, nux_dt(env));
    nu_m4_t view = nu_lookat(nu_v3(70, 70, 70), nu_v3s(0), NU_V3_UP);
    nu_m4_t proj = nu_perspective(nu_radian(60.0),
                                  (nu_f32_t)NUX_SCREEN_WIDTH
                                      / (nu_f32_t)NUX_SCREEN_HEIGHT,
                                  0.5f,
                                  10000);
    nux_render_cubes(env, nu_m4_mul(proj, view).data);

    static nux_u32_t avg_fps = 0;
    static nu_f32_t  sum_fps = 0;
    sum_fps += nux_stat(env, NUX_STAT_FPS);
    if (nux_frame(env) % 10 == 0)
    {
        avg_fps = (nux_u32_t)(sum_fps / 10.);
        sum_fps = 0;
    }

    nux_cursor(env, 0, 0);
    nux_printfmt(env, 7, "FPS:%d", avg_fps);
    nux_printfmt(env, 7, "FRA:%d", nux_frame(env));
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
}
