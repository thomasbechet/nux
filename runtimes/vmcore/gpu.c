#include "gpu.h"
#include "common.h"
#include "memory.h"

static nu_gpu_t gpu;

void
push_gpu_state (nu_byte_t *p)
{
    nu_memcpy(p, &gpu.state, sizeof(gpu.state));
}
void
pop_gpu_state (const nu_byte_t *p)
{
    nu_memcpy(&gpu.state, p, sizeof(gpu.state));
}
