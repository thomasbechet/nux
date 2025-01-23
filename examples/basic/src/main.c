#include "nux.h"
#include <stdio.h>
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

    const nu_v3_t vertices[]
        = { nu_v3(0, 0, 0), nu_v3(1, 0, 0), nu_v3(0, 1, 0) };

    // alloc_texture(0, TEXTURE64, NU_NULL);
    alloc_mesh(0, 3, PRIMITIVE_TRIANGLES, VERTEX_POSTIION, vertices);
}

static nu_f32_t rotation = 0.0f;

void
update (void)
{
    rotation += 0.01;
    nu_m4_t v = nu_lookat(nu_v3(1, 1, -1), NU_V3_ZEROS, NU_V3_UP);
    nu_m4_t p = nu_perspective(nu_radian(60.0), 640.0 / 400.0, 0.01, 100);
    nu_m4_t m = nu_m4_rotate_y(rotation);
    set_transform(TRANSFORM_MODEL, m.data);
    set_transform(TRANSFORM_VIEW, v.data);
    set_transform(TRANSFORM_PROJECTION, p.data);
    draw_mesh(0, m.data);
}
