#include "nux.h"
#define NU_IMPLEMENTATION
#include "nulib.h"

typedef enum
{
    CONTROLLER_FREEFLY_ALIGNED,
    CONTROLLER_FREEFLY,
} controller_mode_t;

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
init_debug_camera (void)
{
    controller.pos   = NU_V3_ZEROS;
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

    nu_v3_t move = NU_V3_ZEROS;
    if (button(0) & BUTTON_Y)
    {
        move.z += 1;
    }
    if (button(0) & BUTTON_A)
    {
        move.z -= 1;
    }
    if (button(0) & BUTTON_B)
    {
        move.x += 1;
    }
    if (button(0) & BUTTON_X)
    {
        move.x -= 1;
    }
    if (button(0) & BUTTON_RB)
    {
        move.y += 1;
    }
    if (button(0) & BUTTON_LB)
    {
        move.y -= 1;
    }
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
            controller.pitch += look.y * dt;
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
    push_transform(TRANSFORM_VIEW, view.data);
    nu_m4_t projection
        = nu_perspective(nu_radian(70.0), 640.0 / 400.0, 0.01, 500);
    push_transform(TRANSFORM_PROJECTION, projection.data);

    if (out_pos)
    {
        *out_pos = pos;
    }
}

static nu_v3_t pos;

void
println (nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nu_char_t buf[256];
    nu_u32_t  n = vsnprintf(buf, sizeof(buf), fmt, args);
    trace(buf, n);
    va_end(args);
}

void
start (void)
{
    // const nu_size_t size = NU_MEM_32M;
    // nu_byte_t      *p0   = malloc(size);
    // *(p0 + size - 1)     = 123;
    // println("%p", p0);
    // println("%d", *(p0 + size - 1));
    // free(p0);

    pos = NU_V3_ZEROS;

    // const nu_f32_t vertices[] = { -0.5, 0,    0, 0, 0, 1, 0, 0, //
    //                               0.5,  0,    0, 0, 1, 0, 1, 0, //
    //                               0,    0.75, 0, 1, 0, 0, 0, 1 };

    // alloc_texture(0, TEXTURE64, NU_NULL);
    // alloc_mesh(0,
    //            3,
    //            PRIMITIVE_TRIANGLES,
    //            VERTEX_POSTIION | VERTEX_UV | VERTEX_COLOR,
    //            vertices);
    // const nu_f32_t colors[] = { 1, 0, 0, 1, 1, 1, 1, 1, 1 };
    // write_mesh(0, VERTEX_COLOR, 0, 3, colors);

    void *p = malloc(NU_MEM_32M);
    println("%p", p);

    init_debug_camera();
}

static nu_f32_t rotation = 0.0f;

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                      \
    ((byte) & 0x80 ? '1' : '0'), ((byte) & 0x40 ? '1' : '0'),     \
        ((byte) & 0x20 ? '1' : '0'), ((byte) & 0x10 ? '1' : '0'), \
        ((byte) & 0x08 ? '1' : '0'), ((byte) & 0x04 ? '1' : '0'), \
        ((byte) & 0x02 ? '1' : '0'), ((byte) & 0x01 ? '1' : '0')

void
update (void)
{
    // rotation += 0.005;

    nu_f32_t scale = 1.5;
    nu_m4_t  m     = nu_m4_translate(nu_v3(0, -5, 0));
    m              = nu_m4_rotate_y(rotation);
    // nu_f32_t scale = 0.5 + ((1 + nu_sin(rotation)) * 0.5) * 0.75;
    m = nu_m4_mul(m, nu_m4_scale(nu_v3s(scale)));
    push_transform(TRANSFORM_MODEL, m.data);
    nu_v3_t pos;
    debug_camera(0.02, &pos);
    draw_model(0);
    draw_model(1);
    m = nu_m4_mul(m, nu_m4_scale(nu_v3(-1, 1, 1)));
    push_transform(TRANSFORM_MODEL, m.data);
    draw_model(1);

    char buf[256];
    sprintf(buf, "x: %.2lf", pos.x);
    draw_text(10, 10, buf);
    sprintf(buf, "y: %.2lf", pos.y);
    draw_text(10, 20, buf);
    sprintf(buf, "z: %.2lf", pos.z);
    draw_text(10, 30, buf);

    sprintf(buf, "c: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(button(0)));
    draw_text(10, 40, buf);
}
