#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "shared.h"

#define MAX_PLAYER 4

typedef enum
{
    GAMEPAD_BUTTON_A  = 1 << 0,
    GAMEPAD_BUTTON_X  = 1 << 1,
    GAMEPAD_BUTTON_Y  = 1 << 2,
    GAMEPAD_BUTTON_B  = 1 << 3,
    GAMEPAD_BUTTON_LB = 1 << 4,
    GAMEPAD_BUTTON_RB = 1 << 5,
} gamepad_button_t;

typedef enum
{
    GAMEPAD_AXIS_LEFTX  = 0,
    GAMEPAD_AXIS_LEFTY  = 1,
    GAMEPAD_AXIS_RIGHTX = 2,
    GAMEPAD_AXIS_RIGHTY = 3,
    GAMEPAD_AXIS_MAX    = 4
} gamepad_axis_t;

typedef struct
{
    nu_u32_t buttons[MAX_PLAYER];
    nu_f32_t axis[MAX_PLAYER][GAMEPAD_AXIS_MAX];
} gamepad_t;

nu_status_t gpad_init(vm_t *vm);
void        gpad_update(vm_t *vm);
nu_u32_t    gpad_button(vm_t *vm, nu_u32_t player);
nu_f32_t    gpad_axis(vm_t *vm, nu_u32_t player, gamepad_axis_t axis);

#endif
