#ifndef NUX_CONFIG_H
#define NUX_CONFIG_H

#include <nulib.h>

#define NUX_RUNTIME_VERSION "0.0.1"

#define NUX_SCREEN_WIDTH  640
#define NUX_SCREEN_HEIGHT 400

typedef struct
{
    nu_u32_t gpu_tex64_unit;
    nu_u32_t gpu_tex128_unit;
    nu_u32_t gpu_tex256_unit;
    nu_u32_t gpu_vert_count;
    nu_u32_t mem_heap_size;
    nu_u32_t mem_stack_size;
} nux_vm_config_t;

typedef enum
{
    NUX_CHUNK_RAW    = 0,
    NUX_CHUNK_WASM   = 1,
    NUX_CHUNK_TEX64  = 2,
    NUX_CHUNK_TEX128 = 3,
    NUX_CHUNK_TEX256 = 4,
} nux_chunk_type_t;

#define NUX_CONFIG_DEFAULT                                               \
    (nux_vm_config_t)                                                    \
    {                                                                    \
        .gpu_tex64_unit = 8, .gpu_tex128_unit = 8, .gpu_tex256_unit = 8, \
        .gpu_vert_count = 1024, .mem_heap_size = NU_MEM_32K,             \
        .mem_stack_size = NU_MEM_1K                                      \
    }

#endif
