#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "api.h"

#define MAX_PLAYER 4

typedef struct
{
    nu_u32_t buttons[MAX_PLAYER];
    nu_f32_t axis[MAX_PLAYER][API_AXIS_MAX];
} gamepad_t;

nu_status_t gpad_init(vm_t *vm);
void        gpad_update(vm_t *vm);

#endif
