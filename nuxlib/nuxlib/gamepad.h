#ifndef NUX_GAMEPAD_H
#define NUX_GAMEPAD_H

#include <nulib/nulib.h>

void nux_gamepad_setup(void);
void nux_draw_gamepad(nu_u32_t player, nu_u32_t x, nu_u32_t y);

#ifdef NUX_IMPLEMENTATION
#include "gamepad_impl.h"
#endif

#endif
