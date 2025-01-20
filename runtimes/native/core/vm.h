#ifndef VM_H
#define VM_H

#include <nulib/nulib.h>

/////////////////
///    CPU    ///
/////////////////

typedef struct
{
    void     *buffer;
    nu_size_t buffer_size;
} cpu_t;

/////////////////
///    IOU    ///
/////////////////

#define IOU_MEM_SIZE NU_MEM_1M

typedef enum
{
    VM_CHUNK_RAW     = 0,
    VM_CHUNK_WASM    = 1,
    VM_CHUNK_TEXTURE = 2,
    VM_CHUNK_MESH    = 3,
} vm_chunk_type_t;

typedef union
{
    struct
    {
        nu_u32_t addr;
    } raw;
    struct
    {
        nu_u32_t slot;
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t w;
        nu_u32_t h;
    } texture;
    struct
    {
        nu_u32_t first;
        nu_u32_t count;
    } mesh;
} vm_chunk_target_t;

typedef struct
{
    vm_chunk_type_t   type;
    vm_chunk_target_t target;
    nu_u32_t          length;
} vm_chunk_header_t;

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} vm_cart_header_t;

typedef struct
{
    vm_cart_header_t header;
    void            *heap;
} iou_t;

/////////////////
///    GPU    ///
/////////////////

#define VM_SCREEN_WIDTH  640
#define VM_SCREEN_HEIGHT 400

#define VM_VERTEX_POSITION_OFFSET 0
#define VM_VERTEX_UV_OFFSET       3 * 4
#define VM_VERTEX_SIZE_F32        5
#define VM_VERTEX_SIZE            VM_VERTEX_SIZE_F32 * 4

#define GPU_MAX_POOL    256
#define GPU_MAX_TEXTURE 1024
#define GPU_MAX_MESH    1024

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
    nu_bool_t active;
    nu_u32_t  size;
    nu_u32_t  remaining;
} gpu_pool_t;

typedef struct
{
    nu_bool_t active;
    nu_u8_t   pool_index;
    nu_u16_t  width;
    nu_u16_t  height;
} gpu_texture_t;

typedef struct
{
    nu_bool_t active;
    nu_u8_t   pool_index;
    nu_u16_t  count;
} gpu_mesh_t;

typedef struct
{
    gpu_pool_t    pools[GPU_MAX_POOL];
    gpu_texture_t textures[GPU_MAX_TEXTURE];
    gpu_mesh_t    meshes[GPU_MAX_MESH];
    gpu_state_t   state;
} gpu_t;

/////////////////
///    VM     ///
/////////////////

#define VM_RUNTIME_VERSION "0.0.1"

#define VM_CONFIG_DEFAULT                                        \
    (vm_config_t)                                                \
    {                                                            \
        .mem_heap_size = NU_MEM_1M, .mem_stack_size = NU_MEM_64K \
    }

typedef struct
{
    nu_u32_t mem_heap_size;
    nu_u32_t mem_stack_size;
} vm_config_t;

typedef struct vm
{
    vm_config_t config;
    cpu_t       cpu;
    gpu_t       gpu;
    iou_t       iou;
    nu_bool_t   running;
} vm_t;

NU_API nu_status_t vm_init(vm_t *vm, const vm_config_t *config);
NU_API void        vm_free(vm_t *vm);
NU_API nu_status_t vm_load(vm_t *vm, const nu_char_t *name);
NU_API nu_status_t vm_tick(vm_t *vm, nu_bool_t *exit);

#endif
