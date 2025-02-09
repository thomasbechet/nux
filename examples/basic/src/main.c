#include "nux.h"
#define NU_IMPLEMENTATION
#include "nulib.h"
#include "camera.h"

static nu_v3_t pos;

void
println (nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nu_char_t buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    trace(buf);
    va_end(args);
}

void
start (void)
{
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

    init_debug_camera(nu_v3(0, 10, 0));
}

static nu_f32_t rotation = 0.0f;
static nu_f32_t time     = 0;

void
update (void)
{
    // rotation += 0.005;
    time += 0.02;
    // clear(0xffffffff);

    clear(0);
    fogdensity(0);
    nu_u32_t c = nu_color_from_vec4(nu_v4(0.5, 0.5, 0.5, 1)).rgba;
    clear(c);
    fogcolor(c);
    // fogrange(1, 50);

    nu_f32_t scale = 1.5;
    nu_m4_t  m     = nu_m4_translate(nu_v3(0, -5, 0));
    m              = nu_m4_rotate_y(rotation);
    m              = nu_m4_mul(m, nu_m4_scale(nu_v3s(scale)));
    transform(TRANSFORM_MODEL, m.data);
    nu_v3_t pos;
    debug_camera(0.02, &pos);
    // draw_model(0);
    draw(1);
    m = nu_m4_mul(m, nu_m4_scale(nu_v3(-1, 1, 1)));
    transform(TRANSFORM_MODEL, m.data);
    draw(1);

    char buf[256];
    sprintf(buf, "x: %.2lf", pos.x);
    cursor(10, 10);
    print(buf);
    sprintf(buf, "y: %.2lf", pos.y);
    print(buf);
    sprintf(buf, "z: %.2lf", pos.z);
    print(buf);

    // blit(1, 0, 0, 128, 128);
}
