#include <nux.h>

#define ALFRED_TEX_SLT 0

static const f32 *orbit_info = (const f32 *)0x3333;

static const f32 vertices[] = { 0,  0, 0, 0, 0, //
                                1,  1, 0, 0, 0, //
                                -1, 0, 0, 0, 0 };

void
start (void)
{
    trace("hello", 5);
    write_vertex(0, 3, vertices);
}

void
update (void)
{
    bind_texture(0);
    draw(0, 3);
    trace("update", 6);
}
