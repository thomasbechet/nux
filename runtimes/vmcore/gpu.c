#include "gpu.h"

#include <nux.h>

static struct
{
    gpu_config_t   config;
    nu_gpu_state_t state;
    nu_u8_t       *cmds;
} _gpu;

void
write_texture (u32 type, u32 slot, const void *p)
{
}
void
write_vertex (u32 first, u32 count, const void *p)
{
}

void
bind_texture (u32 type, u32 slot)
{
}

void
draw (u32 first, u32 count)
{
}
