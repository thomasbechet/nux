#include <nux.h>
#define NU_IMPLEMENTATION
#include <nulib.h>

static void
blitt (nu_u32_t idx, nu_u32_t ts, nu_u32_t x, nu_u32_t y)
{
    blit(idx, x * ts, y * ts, ts, ts);
}

static nu_u32_t pressed[] = {
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
static nu_u32_t released[] = {
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

static void
draw_button (nu_u32_t player, button_t b, nu_u32_t x, nu_u32_t y)
{
    cursor(x, y);
    nu_u32_t        i = nu_log2(b);
    const nu_u32_t *map;
    if (button(player) & b)
    {
        map = pressed;
    }
    else
    {
        map = released;
    }
    blitt(0, 16, map[i * 2 + 0], map[i * 2 + 1]);
}
static void
draw_stick (nu_u32_t player, nu_bool_t left, nu_u32_t x, nu_u32_t y)
{
    nu_v2_t ax      = nu_v2(axis(player, left ? AXIS_LEFTX : AXIS_RIGHTX),
                       axis(player, left ? AXIS_LEFTY : AXIS_RIGHTY));
    ax              = nu_v2_normalize(ax);
    nu_f32_t offset = 3;
    nu_u32_t tx     = left ? 0 : 1;
    nu_u32_t ty     = 8;
    cursor(x, y);
    blitt(0, 16, 2, ty);
    cursor(x + offset * ax.x, y - offset * ax.y);
    blitt(0, 16, tx, ty);
}
static void
draw_trigger (nu_u32_t player, nu_bool_t left, nu_u32_t x, nu_u32_t y)
{
    nu_f32_t ax     = axis(player, left ? AXIS_LT : AXIS_RT);
    nu_f32_t offset = 3;
    nu_u32_t tx     = 6;
    nu_u32_t ty     = left ? 4 : 5;
    cursor(x, y + offset * ax);
    blitt(0, 16, tx, ty);
}
static void
draw_gamepad (nu_u32_t player, nu_u32_t x, nu_u32_t y)
{

    draw_stick(player, NU_TRUE, x - 50, y - 10);
    draw_stick(player, NU_FALSE, x + 20, y + 20);

    draw_trigger(player, NU_TRUE, x - 20, y - 40);
    draw_trigger(player, NU_FALSE, x + 20, y - 40);

    draw_button(player, BUTTON_LB, x - 20, y - 29);
    draw_button(player, BUTTON_RB, x + 20, y - 29);

    {
        nu_v2u_t p      = nu_v2u(x - 20, y + 20);
        nu_u32_t offset = 8;
        draw_button(player, BUTTON_DOWN, p.x, p.y + offset);
        draw_button(player, BUTTON_UP, p.x, p.y - offset);
        draw_button(player, BUTTON_RIGHT, p.x + offset, p.y);
        draw_button(player, BUTTON_LEFT, p.x - offset, p.y);
    }

    {
        nu_v2u_t p      = nu_v2u(x + 50, y - 8);
        nu_u32_t offset = 12;
        draw_button(player, BUTTON_A, p.x, p.y + offset);
        draw_button(player, BUTTON_Y, p.x, p.y - offset);
        draw_button(player, BUTTON_B, p.x + offset, p.y);
        draw_button(player, BUTTON_X, p.x - offset, p.y);
    }
}

void
start (void)
{
}

void
update (void)
{
    clear(NU_COLOR_GREY.rgba);
    draw_gamepad(0, 100, 100);
    draw_gamepad(1, 100, 225);
}
