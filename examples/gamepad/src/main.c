#include <nux.h>
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>
#define NUX_IMPLEMENTATION
#include <nuxlib/gamepad.h>

void
start (void)
{
    nux_setup_gamepad(0, 0);
}

void
update (void)
{
    clear(NU_COLOR_GREY.rgba);
    nux_draw_gamepad(0, 100, 100);
    nux_draw_gamepad(1, 100, 225);
}
