#include "freecam.h"

#include <nux.h>

static struct
{
    nu_u32_t player;

    nu_v3_t  pos;
    nu_v3_t  vel;
    nu_v3_t  acc;
    nu_q4_t  rot;
    nu_f32_t yaw;
    nu_f32_t pitch;

    nux_freecam_mode_t mode;

    nu_f32_t fov;
    nu_f32_t speed;
} nux__freecam;

void
nux_init_debug_camera (nu_v3_t pos)
{
    nux__freecam.player = 0;

    nux__freecam.pos   = pos;
    nux__freecam.vel   = NU_V3_ZEROS;
    nux__freecam.acc   = NU_V3_ZEROS;
    nux__freecam.rot   = nu_q4_identity();
    nux__freecam.pitch = 0;
    nux__freecam.yaw   = 0;

    nux__freecam.mode = NUX_FREECAM_FREEFLY_ALIGNED;

    nux__freecam.fov   = 90;
    nux__freecam.speed = 50;
}

void
nux_debug_camera (nu_f32_t dt, nu_v3_t *out_pos)
{
    nu_v3_t look = nu_v3(axis(nux__freecam.player, AXIS_RIGHTX) * 100,
                         axis(nux__freecam.player, AXIS_RIGHTY) * 100,
                         0);

    nu_v3_t move = nu_v3(axis(nux__freecam.player, AXIS_LEFTX),
                         0,
                         axis(nux__freecam.player, AXIS_LEFTY));
    move.y += button(nux__freecam.player) & BUTTON_Y ? 1 : 0;
    move.y -= button(nux__freecam.player) & BUTTON_B ? 1 : 0;
    move = nu_v3_normalize(move);

    // Switch mode
    if (NU_FALSE)
    {
        switch (nux__freecam.mode)
        {
            case NUX_FREECAM_FREEFLY_ALIGNED:
                nux__freecam.mode = NUX_FREECAM_FREEFLY;
                break;
            case NUX_FREECAM_FREEFLY:
                nux__freecam.mode = NUX_FREECAM_FREEFLY_ALIGNED;
                break;
        }
    }

    // Translation
    nu_v3_t direction = NU_V3_ZEROS;

    direction = nu_v3_add(
        direction,
        nu_v3_muls(nu_q4_mulv3(nux__freecam.rot, NU_V3_FORWARD), move.z));
    direction = nu_v3_add(
        direction,
        nu_v3_muls(nu_q4_mulv3(nux__freecam.rot, NU_V3_RIGHT), move.x));
    if (nux__freecam.mode == NUX_FREECAM_FREEFLY)
    {
        direction = nu_v3_add(
            direction,
            nu_v3_muls(nu_q4_mulv3(nux__freecam.rot, NU_V3_UP), move.y));
    }
    else if (nux__freecam.mode == NUX_FREECAM_FREEFLY_ALIGNED)
    {
        direction = nu_v3_add(direction, nu_v3_muls(NU_V3_UP, move.y));
    }
    else
    {
        // No vertical movement in character mode
    }

    direction = nu_v3_normalize(direction);

    // Rotation
    if (nux__freecam.mode == NUX_FREECAM_FREEFLY)
    {
        if (look.x != 0)
        {
            nux__freecam.rot = nu_q4_mul_axis(
                nux__freecam.rot, NU_V3_UP, -nu_radian(look.x) * dt);
        }
        if (look.y != 0)
        {
            nux__freecam.rot = nu_q4_mul_axis(
                nux__freecam.rot, NU_V3_RIGHT, -nu_radian(look.y) * dt);
        }
        if (look.z != 0)
        {
            nux__freecam.rot = nu_q4_mul_axis(
                nux__freecam.rot, NU_V3_FORWARD, -nu_radian(look.z) * dt);
        }
    }
    else
    {
        if (look.x != 0)
        {
            nux__freecam.yaw += look.x * dt;
        }
        if (look.y != 0)
        {
            nux__freecam.pitch -= look.y * dt;
        }
        nux__freecam.pitch = NU_CLAMP(nux__freecam.pitch, -90.0, 90.0);
        nux__freecam.rot   = nu_q4_axis(NU_V3_UP, -nu_radian(nux__freecam.yaw));
        nux__freecam.rot   = nu_q4_mul(
            nux__freecam.rot,
            nu_q4_axis(NU_V3_RIGHT, -nu_radian(nux__freecam.pitch)));
    }

    // Compute sum of forces
    const nu_f32_t mass  = 10.0;
    nu_v3_t        force = NU_V3_ZEROS;

    // Apply movement
    if (nu_v3_norm(direction) > 0.001)
    {
        force = nu_v3_add(force, nu_v3_muls(direction, 8));
    }

    // Apply drag
    force = nu_v3_add(force, nu_v3_muls(nux__freecam.vel, -0.3));

    // Integrate
    nux__freecam.pos
        = nu_v3_add(nux__freecam.pos,
                    nu_v3_add(nu_v3_muls(nux__freecam.vel, dt),
                              nu_v3_muls(nux__freecam.acc, 0.5 * dt * dt)));
    nu_v3_t new_acc = nu_v3_muls(force, mass);
    nu_v3_t new_vel
        = nu_v3_add(nux__freecam.vel,
                    nu_v3_muls(nu_v3_add(nux__freecam.acc, new_acc), 0.5 * dt));
    if (nu_v3_norm(new_vel) < 0.1)
    {
        new_vel = NU_V3_ZEROS;
    }
    nux__freecam.acc = new_acc;
    nux__freecam.vel = new_vel;

    nu_v3_t pos     = nux__freecam.pos;
    nu_v3_t forward = nu_q4_mulv3(nux__freecam.rot, NU_V3_FORWARD);
    nu_v3_t up      = nu_v3_normalize(nu_q4_mulv3(nux__freecam.rot, NU_V3_UP));

    nu_m4_t view = nu_lookat(pos, nu_v3_add(pos, forward), up);
    transform(TRANSFORM_VIEW, view.data);
    nu_m4_t projection
        = nu_perspective(nu_radian(70.0), 640.0 / 400.0, 0.01, 500);
    transform(TRANSFORM_PROJECTION, projection.data);

    if (out_pos)
    {
        *out_pos = pos;
    }
}
