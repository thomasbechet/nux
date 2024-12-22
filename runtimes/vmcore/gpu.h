#ifndef NU_GPU_H
#define NU_GPU_H

#include <nulib.h>

typedef struct
{
    nu_u32_t texture;
    nu_u32_t buffer;
    nu_v4u_t scissor;
    nu_v4u_t viewport;
    nu_m4_t  model;
    nu_u32_t mode;
    nu_u8_t *cmds;
} nu_gpu_state_t;

#endif
