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
    freecam.speed = 50;
}

static void
debug_camera (nux_env_t env, nu_f32_t dt)
{
    // nux_f32_t vel = nu_v3_norm(freecam.vel);
    // nux_f32_t bob = nu_sin(nux_time(env) * 0.1 * vel * (2 * NU_PI));
    nux_f32_t bob = 0;

    nu_v3_t look = nu_v3(nux_axis(env, freecam.player, NUX_AXIS_RIGHTX) * 100,
                         nux_axis(env, freecam.player, NUX_AXIS_RIGHTY) * 100,
                         0);
    nu_v3_t move = nu_v3(nux_axis(env, freecam.player, NUX_AXIS_LEFTX),
                         0,
                         nux_axis(env, freecam.player, NUX_AXIS_LEFTY));
    move.y += nux_button(env, freecam.player) & NUX_BUTTON_Y ? 1 : 0;
    move.y -= nux_button(env, freecam.player) & NUX_BUTTON_B ? 1 : 0;
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
        force = nu_v3_add(force, nu_v3_muls(direction, 3));
    }

    // Apply drag
    force = nu_v3_add(force, nu_v3_muls(freecam.vel, -1));

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

    nu_v3_t pos    = freecam.pos;
    nu_v3_t center = nu_v3_add(pos, nu_q4_mulv3(freecam.rot, NU_V3_FORWARD));
    nu_v3_t up     = nu_v3_normalize(nu_q4_mulv3(freecam.rot, NU_V3_UP));

    nux_cameye(env, pos.x + bob * 0.1, pos.y + bob * 0.1, pos.z);
    nux_camcenter(env, center.x, center.y, center.z);
    nux_camup(env, up.x, up.y, up.z);
}

static void
blit_colormap (nux_env_t env)
{
    for (int i = 0; i < NUX_PALETTE_SIZE; ++i)
    {
        const nux_u32_t tw     = 30;
        const nux_u32_t th     = 10;
        const nux_u32_t tr     = 5;
        nux_u32_t       x      = i % tr;
        nux_u32_t       y      = i / tr;
        nux_u32_t       origin = NUX_CANVAS_WIDTH - tw * tr;
        nux_u32_t       px     = x * tw;
        nux_u32_t       py     = y * th;
        nux_rectfill(env,
                     origin + px,
                     py,
                     origin + (x + 1) * tw - 1,
                     (y + 1) * th - 1,
                     i);
        nux_textfmt(env, origin + px + 1, py, 7, "%d", i);
    }
}

#ifdef NUX_BENCHMARK
#include <time.h>
#endif

void
loop_init (nux_env_t env)
{
    load_blk_colormap(env);
    init_debug_camera(NU_V3_ZEROS);
}
void
loop_update (nux_env_t env)
{
    nux_cls(env, 0);

#ifdef NUX_BENCHMARK
    clock_t t;
    t = clock();
#endif

    nu_m4_t   model;
    nux_i32_t count = 10;
    nux_dbgi32(env, "count", &count);
    for (int i = 0; i < count; ++i)
    {
        model = nu_m4_mul(nu_m4_translate(nu_v3(1.2 * (i % 5), 0, (i / 5) * 2)),
                          nu_m4_scale(NU_V3_ONES));
        // draw_red(env, model.data, 1);
    }
    nux_camfov(env, 50);
    for (int i = 1; i < 10; ++i)
    {
        model = nu_m4_translate(nu_v3(0, 1, i * 5));
        nux_draw_cube(env, 2, 2, 0.1, model.data);
    }
    model = nu_m4_translate(nu_v3(-8, 0, 0));
    // draw_krabe(env, model.data, 0);
    model = nu_m4_translate(nu_v3(-3, 0, 0));
    // draw_krabe(env, model.data, 1);
    nux_f32_t y = 0.6;
    nux_dbgf32(env, "y", &y);
    model = nu_m4_mul(nu_m4_translate(nu_v3(0, y, 0.5)),
                      nu_m4_scale(nu_v3s(0.08)));
    // draw_lavalamp(env, model.data, 1);

#ifdef NUX_BENCHMARK
    static double sum   = 0;
    static int    frame = 0;
    t                   = clock() - t;
    sum += ((double)t) / CLOCKS_PER_SEC; // in seconds
    ++frame;
    if (frame == 100)
    {
        printf("%lf\n", sum / 100);
        frame = 0;
        sum   = 0;
        exit(0);
    }
#endif

#ifdef NUX_BENCHMARK
    nux_campos(env, 70, 70, 70);
    nux_camcenter(env, 0, 0, 0);
#else
    debug_camera(env, nux_dt(env));
#endif
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
    nux_printfmt(env, 7, "RES:%dx%d", NUX_CANVAS_WIDTH, NUX_CANVAS_HEIGHT);
    nux_printfmt(env, 7, "TRI:%d", nux_tricount(env));

    // blit_colormap(env);
}
