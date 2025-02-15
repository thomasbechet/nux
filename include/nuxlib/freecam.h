#ifndef NUX_FREECRAM_H
#define NUX_FREECRAM_H

#include <nulib/nulib.h>

typedef enum
{
    NUX_FREECAM_FREEFLY_ALIGNED,
    NUX_FREECAM_FREEFLY,
} nux_freecam_mode_t;

void nux_init_debug_camera(nu_v3_t pos);
void nux_debug_camera(nu_f32_t dt, nu_v3_t *out_pos);

#ifdef NUX_IMPLEMENTATION
#include "freecam_impl.h"
#endif

#endif
