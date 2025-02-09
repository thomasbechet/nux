#ifndef CAMERA_H
#define CAMERA_H

#include "nulib.h"

typedef enum
{
    CONTROLLER_FREEFLY_ALIGNED,
    CONTROLLER_FREEFLY,
} controller_mode_t;

void init_debug_camera(nu_v3_t pos);
void debug_camera(nu_f32_t dt, nu_v3_t *out_pos);

#endif
