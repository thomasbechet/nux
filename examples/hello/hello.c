#include <nux.h>

#define ALFRED_TEX_SLT 0

static const f32 *orbit_info = (const f32 *)0x3333;

void
start (void)
{
    trace("hello", 5);
}

void
update (void)
{
    draw(0, 3);
}
