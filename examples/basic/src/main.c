#include "nux.h"
#define NU_IMPLEMENTATION
#include "nulib.h"

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
    const nu_size_t size = NU_MEM_32M;
    nu_byte_t      *p0   = malloc(size);
    *(p0 + size - 1)     = 123;
    println("%p", p0);
    println("%d", *(p0 + size - 1));
    free(p0);

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

    for (int i = 0; i < 5; ++i)
    {
        set_model_texture(i, 0);
    }
}

static nu_f32_t rotation = 0.0f;

void
update (void)
{
    rotation += 0.03;

    nu_m4_t m = nu_m4_translate(nu_v3(0, -5, 0));
    set_model_transform(0, m.data);

    nu_m4_t v      = nu_lookat(nu_v3(50, 3, 0), nu_v3(0, 15, 0), NU_V3_UP);
    nu_m4_t p      = nu_perspective(nu_radian(50.0), 640.0 / 400.0, 0.01, 100);
    m              = nu_m4_rotate_y(rotation);
    nu_f32_t scale = 0.5 + ((1 + nu_sin(rotation)) * 0.5) * 0.75;
    m              = nu_m4_mul(m, nu_m4_scale(nu_v3s(scale)));
    set_transform(TRANSFORM_MODEL, m.data);
    set_transform(TRANSFORM_VIEW, v.data);
    set_transform(TRANSFORM_PROJECTION, p.data);
    draw_model(0);
}
