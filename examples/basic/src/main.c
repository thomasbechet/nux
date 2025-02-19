#include "nux.h"
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>
#define NUX_IMPLEMENTATION
#include <nuxlib/freecam.h>
#include <nuxlib/gamepad.h>
#include <nuxlib/print.h>
#include <nuxlib/debug.h>

#define MODEL_INDUSTRIAL 4
#define MODEL_ARIANE6    3

static nu_f32_t rotation = 0.0f;

void
start (void)
{
    nux_gamepad_setup(666, 5);
    nux_init_debug_camera(nu_v3(0, 10, 0));
}

void
update (void)
{
    nu_u32_t c = nu_color_from_vec4(nu_v4(0.7, 0.7, 0.7, 1)).rgba;
    clear(c);
    nu_f32_t       near  = 100 + nu_sin(time()) * 10.0;
    const nu_f32_t fog[] = { 1, near, near + 30 };
    fog_params(fog);
    clear(c);
    fog_color(c);

    nu_f32_t scale = 1.5;
    nu_m4_t  m     = nu_m4_translate(nu_v3(0, -5, 0));
    m              = nu_m4_rotate_y(rotation);
    m              = nu_m4_mul(m, nu_m4_scale(nu_v3s(scale)));
    transform(TRANSFORM_MODEL, m.data);
    nu_v3_t pos;
    nux_debug_camera(0.02, &pos);
    // draw_model(0);
    draw(MODEL_INDUSTRIAL);
    m = nu_m4_mul(m, nu_m4_scale(nu_v3(-1, 1, 1)));
    transform(TRANSFORM_MODEL, m.data);
    draw(MODEL_INDUSTRIAL);

    const nu_size_t count = 1000;
    nu_f32_t        points[count * 3];
    for (nu_size_t i = 0; i < count; ++i)
    {
        points[i * 3 + 0] = nu_cos(i * 0.3);
        points[i * 3 + 1] = i * 0.05 * (1 + nu_sin(time())) * 0.5;
        points[i * 3 + 2] = nu_sin(i * 0.3);
    }
    color(0xFF0000FF);
    draw_lines(points, count);
    const nu_f32_t center[] = { 0, 0, 0 };
    const nu_f32_t size[]   = { 1, 1, 1 };
    draw_cube(center, size);
    color(0xFFFFFFFF);

    char buf[256];
    sprintf(buf, "x: %.2lf", pos.x);
    cursor(10, 10);
    print(buf);
    sprintf(buf, "y: %.2lf", pos.y);
    print(buf);
    sprintf(buf, "z: %.2lf", pos.z);
    print(buf);
    sprintf(buf, "n: %.2lf", near);
    print(buf);
    sprintf(buf, "t: %.2lf", time());
    print(buf);
    nu_f32_t memusage = (nu_f32_t)console_info(CONSOLE_MEMORY_USAGE)
                        / (nu_f32_t)console_info(CONSOLE_MEMORY_CAPACITY);
    sprintf(buf, "m: %.2lf%%", memusage * 100);
    print(buf);

    sprintf(buf, "b: %d", button(0));
    print(buf);

    m = nu_m4_translate(nu_v3(10, 0, 0));
    transform(TRANSFORM_MODEL, m.data);
    color(NU_COLOR_BLUE.rgba);
    draw(MODEL_ARIANE6);
    color(0xFFFFFFFF);

    nux_debug();

    // blit(1, 0, 0, 128, 128);

    nux_draw_gamepad(0, 85, 200);
}
