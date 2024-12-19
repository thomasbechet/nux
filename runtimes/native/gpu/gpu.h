#ifndef NU_GPU_H
#define NU_GPU_H

#include <core/core.h>

typedef struct
{
    nu_u32_t texture;
    nu_u32_t buffer;
    nu_v4u_t scissor;
    nu_v4u_t viewport;
    nu_m4_t  model;
    nu_u32_t mode;
} nu_gpu_state_t;

typedef struct
{
    nu_gpu_state_t state;
} nu_gpu_t;

#endif
