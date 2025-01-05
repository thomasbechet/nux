#ifndef NUX_CONFIG_H
#define NUX_CONFIG_H

#include <nulib.h>

#define NUX_RUNTIME_VERSION "0.0.1"

#define NUX_SCREEN_WIDTH  640
#define NUX_SCREEN_HEIGHT 400

#define NUX_TEXTURE_SIZE      1024
#define NUX_TEXTURE_DATA_SIZE (1024 * 1024 * 4)

#define NUX_VERTEX_POSITION_OFFSET 0
#define NUX_VERTEX_UV_OFFSET       3 * 4
#define NUX_VERTEX_SIZE_F32        5
#define NUX_VERTEX_SIZE            NUX_VERTEX_SIZE_F32 * 4

typedef struct
{
    nu_u32_t gpu_texture_count;
    nu_u32_t gpu_vertex_count;
    nu_u32_t mem_heap_size;
    nu_u32_t mem_stack_size;
} nux_vm_config_t;

typedef enum
{
    NUX_CHUNK_RAW     = 0,
    NUX_CHUNK_WASM    = 1,
    NUX_CHUNK_TEXTURE = 2,
    NUX_CHUNK_MESH    = 3,
} nux_chunk_type_t;

#define NUX_CONFIG_DEFAULT                                       \
    (nux_vm_config_t)                                            \
    {                                                            \
        .gpu_texture_count = 4, .gpu_vertex_count = 1024,        \
        .mem_heap_size = NU_MEM_32K, .mem_stack_size = NU_MEM_1K \
    }

#endif
