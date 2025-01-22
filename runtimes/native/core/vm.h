#ifndef VM_H
#define VM_H

#include <nulib/nulib.h>

/////////////////
///    CPU    ///
/////////////////
///
typedef struct
{
    nu_u32_t mem_heap_size;
    nu_u32_t mem_stack_size;
} cpu_config_t;

typedef struct
{
    cpu_config_t config;
    void        *buffer;
    nu_size_t    buffer_size;
} cpu_t;

/////////////////
///    GPU    ///
/////////////////

#define VM_SCREEN_WIDTH  640
#define VM_SCREEN_HEIGHT 400

#define VM_VERTEX_POSITION_OFFSET 0
#define VM_VERTEX_UV_OFFSET       3 * 4
#define VM_VERTEX_SIZE_F32        5
#define VM_VERTEX_SIZE            VM_VERTEX_SIZE_F32 * 4

#define GPU_MAX_TEXTURE 1024
#define GPU_MAX_VBUFFER 1024

typedef enum
{
    GPU_TEXTURE64  = 0,
    GPU_TEXTURE128 = 1,
    GPU_TEXTURE256 = 2,
    GPU_TEXTURE512 = 3,
} gpu_texture_size_t;

typedef struct
{
    nu_u32_t max_vertex_count;
    nu_u32_t max_texture_counts[4];
} gpu_config_t;

typedef struct
{
    nu_u32_t texture;
    nu_u32_t buffer;
    nu_v4u_t scissor;
    nu_v4u_t viewport;
    nu_m4_t  model;
    nu_u32_t mode;
    nu_u8_t *cmds;
} gpu_state_t;

typedef struct
{
    nu_bool_t          active;
    gpu_texture_size_t size;
} gpu_texture_t;

typedef struct
{
    nu_bool_t active;
    nu_u32_t  count;
} gpu_vbuffer_t;

typedef struct
{
    gpu_state_t   state;
    gpu_config_t  config;
    gpu_texture_t textures[GPU_MAX_TEXTURE];
    gpu_vbuffer_t vbuffers[GPU_MAX_VBUFFER];
} gpu_t;

/////////////////
///    IOU    ///
/////////////////

#define IOU_MEM_SIZE NU_MEM_1M

typedef enum
{
    CART_CHUNK_RAW     = 0,
    CART_CHUNK_WASM    = 1,
    CART_CHUNK_TEXTURE = 2,
    CART_CHUNK_VBUFFER = 3,
} cart_chunk_type_t;

typedef union
{
    struct
    {
        nu_u32_t addr;
    } raw;
    struct
    {
        nu_u32_t           index;
        gpu_texture_size_t size;
    } texture;
    struct
    {
        nu_u32_t index;
        nu_u16_t count;
    } vbuffer;
} cart_chunk_meta_t;

typedef struct
{
    cart_chunk_type_t type;
    cart_chunk_meta_t meta;
    nu_u32_t          length;
} cart_chunk_header_t;

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} cart_header_t;

typedef struct
{
    cart_header_t header;
    void         *heap;
} iou_t;

/////////////////
///    VM     ///
/////////////////

#define VM_RUNTIME_VERSION "0.0.1"

#define VM_CONFIG_DEFAULT                                                \
    (vm_config_t)                                                        \
    {                                                                    \
        .cpu.mem_heap_size = NU_MEM_1M, .cpu.mem_stack_size = NU_MEM_64K \
    }

typedef struct
{
    cpu_config_t cpu;
    gpu_config_t gpu;
} vm_config_t;

typedef struct vm
{
    cpu_t     cpu;
    gpu_t     gpu;
    iou_t     iou;
    nu_bool_t running;
} vm_t;

NU_API nu_status_t vm_init(vm_t *vm, const vm_config_t *config);
NU_API void        vm_free(vm_t *vm);
NU_API nu_status_t vm_load(vm_t *vm, const nu_char_t *name);
NU_API nu_status_t vm_tick(vm_t *vm, nu_bool_t *exit);

#endif
