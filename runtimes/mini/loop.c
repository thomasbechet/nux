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

nu_m4_t
debug_camera (nux_env_t env, nu_f32_t dt)
{
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
        force = nu_v3_add(force, nu_v3_muls(direction, 9));
    }

    // Apply drag
    force = nu_v3_add(force, nu_v3_muls(freecam.vel, -0.2f));

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

void
loop_init (nux_env_t env)
{
    nux_cset(env, 0, 0x0B3954);
    nux_cset(env, 1, 0xBFD7EA);
    nux_cset(env, 2, 0xFF6663);
    nux_cset(env, 3, 0xFF0000);
    nux_cset(env, 4, 0x00FF00);
    nux_cset(env, 5, 0x0000FF);
    nux_cset(env, 6, 0);
    nux_cset(env, 7, 0xFFFFFF);
    init_debug_camera(NU_V3_ZEROS);
}
void
loop_update (nux_env_t env)
{
    nux_cls(env, 0);
    nux_clsz(env);
    nu_v2i_t v0 = nu_v2i(50, 50);
    nu_v2i_t v1 = nu_v2i(100 + nu_cos(nux_time(env) * 0.5) * 50,
                         70 + nu_sin(nux_time(env) * 0.5) * 50);
    nu_v2i_t v2 = nu_v2i(80, 200);
    nux_trifill(env, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, 1);
    nux_line(env, 150, 150, 300, 20, 2);
    // circle(env, 100, 100, 50, 3);
    // plot_circle(env, 100, 100, 50, 3);

#ifdef NUX_BENCHMARK
    nu_m4_t view = nu_lookat(nu_v3(70, 70, 70), nu_v3s(0), NU_V3_UP);
    nu_m4_t proj = nu_perspective(nu_radian(60.0),
                                  (nu_f32_t)NUX_SCREEN_WIDTH
                                      / (nu_f32_t)NUX_SCREEN_HEIGHT,
                                  0.5f,
                                  10000);
    nu_m4_t vp   = nu_m4_mul(proj, view);
#else
    nu_m4_t vp = debug_camera(env, nux_dt(env));
#endif
    nux_render_cubes(env, vp.data);

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
    nux_printfmt(env, 7, "RES:%dx%d", NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT);

    nux_circ(env, 50, 50, 10, 5);
    nux_rectfill(env, 200, 100, 250, 150, 2);
    nux_rect(env, 200, 100, 250, 150, 1);
    nux_text(env, 203, 101, "Hello", 7);
}
