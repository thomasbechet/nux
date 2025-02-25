#include "gamepad.h"

#include <nux.h>

static nu_u32_t nux__pressed[] = {
    3, 1, // A
    3, 0, // Y
    3, 2, // B
    3, 3, // X
    2, 4, // UP
    3, 4, // DOWN
    2, 5, // LEFT
    3, 5, // RIGHT
    8, 6, // LB
    8, 7, // RB
};
static nu_u32_t nux__released[] = {
    1, 1, // A
    1, 0, // Y
    1, 2, // B
    1, 3, // X
    0, 6, // UP
    1, 6, // DOWN
    0, 7, // LEFT
    1, 7, // RIGHT
    6, 6, // LB
    6, 7, // RB
};

static nu_u32_t nux__gamepad_spriteheet;

static void
nux__blitt (nu_u32_t x, nu_u32_t y)
{
    draw_sprite(nux__gamepad_spriteheet, y * 10 + x);
}
static void
nux__draw_button (nu_u32_t player, button_t b, nu_u32_t x, nu_u32_t y)
{
    set_cursor(x, y);
    nu_u32_t        i = (nu_u32_t)nu_log2(b);
    const nu_u32_t *map;
    if (button(player) & b)
    {
        map = nux__pressed;
    }
    else
    {
        map = nux__released;
    }
    nux__blitt(map[i * 2 + 0], map[i * 2 + 1]);
}
static void
nux__draw_stick (nu_u32_t player, nu_bool_t left, nu_u32_t x, nu_u32_t y)
{
    nu_v2_t ax = nu_v2(axis(player, left ? AXIS_LEFTX : AXIS_RIGHTX),
                       axis(player, left ? AXIS_LEFTY : AXIS_RIGHTY));
    if (nu_v2_norm(ax) < 0.1)
    {
        ax = NU_V2_ZEROS;
    }
    ax              = nu_v2_normalize(ax);
    nu_f32_t offset = 3;
    nu_u32_t tx     = left ? 0 : 1;
    nu_u32_t ty     = 8;
    set_cursor(x, y);
    nux__blitt(2, ty);
    set_cursor(x + (nu_u32_t)(offset * ax.x), y - (nu_u32_t)(offset * ax.y));
    nux__blitt(tx, ty);
}
static void
nux__draw_trigger (nu_u32_t player, nu_bool_t left, nu_u32_t x, nu_u32_t y)
{
    nu_f32_t ax     = axis(player, left ? AXIS_LT : AXIS_RT);
    nu_f32_t offset = 3;
    nu_u32_t tx     = 6;
    nu_u32_t ty     = left ? 4 : 5;
    set_cursor(x, y + (nu_u32_t)(offset * ax));
    nux__blitt(tx, ty);
}

void
nux_gamepad_setup (nu_u32_t spritesheet, nu_u32_t texture)
{
    nux__gamepad_spriteheet = spritesheet;
    init_spritesheet(spritesheet, texture, 10, 9, 16, 16);
}
void
nux_draw_gamepad (nu_u32_t player, nu_u32_t x, nu_u32_t y)
{
    x -= 28;
    y -= 8;

    nux__draw_stick(player, NU_TRUE, x - 50, y - 10);
    nux__draw_stick(player, NU_FALSE, x + 20, y + 20);

    nux__draw_trigger(player, NU_TRUE, x - 20, y - 40);
    nux__draw_trigger(player, NU_FALSE, x + 20, y - 40);

    nux__draw_button(player, BUTTON_LB, x - 20, y - 29);
    nux__draw_button(player, BUTTON_RB, x + 20, y - 29);

    {
        nu_v2u_t p      = nu_v2u(x - 20, y + 20);
        nu_u32_t offset = 8;
        nux__draw_button(player, BUTTON_DOWN, p.x, p.y + offset);
        nux__draw_button(player, BUTTON_UP, p.x, p.y - offset);
        nux__draw_button(player, BUTTON_RIGHT, p.x + offset, p.y);
        nux__draw_button(player, BUTTON_LEFT, p.x - offset, p.y);
    }

    {
        nu_v2u_t p      = nu_v2u(x + 50, y - 8);
        nu_u32_t offset = 12;
        nux__draw_button(player, BUTTON_A, p.x, p.y + offset);
        nux__draw_button(player, BUTTON_Y, p.x, p.y - offset);
        nux__draw_button(player, BUTTON_B, p.x + offset, p.y);
        nux__draw_button(player, BUTTON_X, p.x - offset, p.y);
    }
}
