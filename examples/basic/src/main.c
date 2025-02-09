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

    sprintf(buf, "c: " BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(button(0)));
    print(buf);

    // blit(1, 0, 0, 128, 128);
}
