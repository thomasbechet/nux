#include <nux.h>
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>
#define NUX_IMPLEMENTATION
#include <nuxlib/gamepad.h>

void
start (void)
{
    nux_gamepad_setup(5, 2);
}

void
update (void)
{
    clear(NU_COLOR_GREY.rgba);
    nux_draw_gamepad(0, 360 / 2, 240 / 2);
}
