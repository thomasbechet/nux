#include "camera.h"

#include <nux.h>

static struct
{
    nu_v3_t  pos;
    nu_v3_t  vel;
    nu_v3_t  acc;
    nu_q4_t  rot;
    nu_f32_t yaw;
    nu_f32_t pitch;

    controller_mode_t mode;

    nu_f32_t fov;
    nu_f32_t speed;
} controller;

void
init_debug_camera (nu_v3_t pos)
{
    controller.pos   = pos;
    controller.vel   = NU_V3_ZEROS;
    controller.acc   = NU_V3_ZEROS;
    controller.rot   = nu_q4_identity();
    controller.pitch = 0;
    controller.yaw   = 0;

    controller.mode = CONTROLLER_FREEFLY_ALIGNED;

    controller.fov   = 90;
    controller.speed = 50;
}

void
debug_camera (nu_f32_t dt, nu_v3_t *out_pos)
{
    nu_v3_t look
        = nu_v3(axis(0, AXIS_RIGHTX) * 100, axis(0, AXIS_RIGHTY) * 100, 0);

    nu_v3_t move = nu_v3(axis(0, AXIS_LEFTX), 0, axis(0, AXIS_LEFTY));
    move.y += button(0) & BUTTON_Y ? 1 : 0;
    move.y -= button(0) & BUTTON_B ? 1 : 0;
    move = nu_v3_normalize(move);

    // Switch mode
    if (NU_FALSE)
    {
        switch (controller.mode)
        {
            case CONTROLLER_FREEFLY_ALIGNED:
                controller.mode = CONTROLLER_FREEFLY;
                break;
            case CONTROLLER_FREEFLY:
                controller.mode = CONTROLLER_FREEFLY_ALIGNED;
                break;
        }
    }

    // Translation
    nu_v3_t direction = NU_V3_ZEROS;

    direction = nu_v3_add(
        direction,
        nu_v3_muls(nu_q4_mulv3(controller.rot, NU_V3_FORWARD), move.z));
    direction = nu_v3_add(
        direction,
        nu_v3_muls(nu_q4_mulv3(controller.rot, NU_V3_RIGHT), move.x));
    if (controller.mode == CONTROLLER_FREEFLY)
    {
        direction = nu_v3_add(
            direction,
            nu_v3_muls(nu_q4_mulv3(controller.rot, NU_V3_UP), move.y));
    }
    else if (controller.mode == CONTROLLER_FREEFLY_ALIGNED)
    {
        direction = nu_v3_add(direction, nu_v3_muls(NU_V3_UP, move.y));
    }
    else
    {
        // No vertical movement in character mode
    }

    direction = nu_v3_normalize(direction);

    // Rotation
    if (controller.mode == CONTROLLER_FREEFLY)
    {
        if (look.x != 0)
        {
            controller.rot = nu_q4_mul_axis(
                controller.rot, NU_V3_UP, -nu_radian(look.x) * dt);
        }
        if (look.y != 0)
        {
            controller.rot = nu_q4_mul_axis(
                controller.rot, NU_V3_RIGHT, -nu_radian(look.y) * dt);
        }
        if (look.z != 0)
        {
            controller.rot = nu_q4_mul_axis(
                controller.rot, NU_V3_FORWARD, -nu_radian(look.z) * dt);
        }
    }
    else
    {
        if (look.x != 0)
        {
            controller.yaw += look.x * dt;
        }
        if (look.y != 0)
        {
            controller.pitch -= look.y * dt;
        }
        controller.pitch = NU_CLAMP(controller.pitch, -90.0, 90.0);
        controller.rot   = nu_q4_axis(NU_V3_UP, -nu_radian(controller.yaw));
        controller.rot
            = nu_q4_mul(controller.rot,
                        nu_q4_axis(NU_V3_RIGHT, -nu_radian(controller.pitch)));
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
    force = nu_v3_add(force, nu_v3_muls(controller.vel, -0.3));

    // Integrate
    controller.pos
        = nu_v3_add(controller.pos,
                    nu_v3_add(nu_v3_muls(controller.vel, dt),
                              nu_v3_muls(controller.acc, 0.5 * dt * dt)));
    nu_v3_t new_acc = nu_v3_muls(force, mass);
    nu_v3_t new_vel
        = nu_v3_add(controller.vel,
                    nu_v3_muls(nu_v3_add(controller.acc, new_acc), 0.5 * dt));
    if (nu_v3_norm(new_vel) < 0.1)
    {
        new_vel = NU_V3_ZEROS;
    }
    controller.acc = new_acc;
    controller.vel = new_vel;

    nu_v3_t pos     = controller.pos;
    nu_v3_t forward = nu_q4_mulv3(controller.rot, NU_V3_FORWARD);
    nu_v3_t up      = nu_v3_normalize(nu_q4_mulv3(controller.rot, NU_V3_UP));

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
