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

// #define VM_SCREEN_WIDTH  640
// #define VM_SCREEN_HEIGHT 400
// #define VM_SCREEN_WIDTH  512
// #define VM_SCREEN_HEIGHT 288
#define VM_SCREEN_WIDTH  320
#define VM_SCREEN_HEIGHT 200

#define GPU_MAX_TEXTURE 1024
#define GPU_MAX_MESH    1024
#define GPU_MAX_NODE    1024

typedef enum
{
    GPU_TEX64  = 0,
    GPU_TEX128 = 1,
    GPU_TEX256 = 2,
    GPU_TEX512 = 3,
} gpu_texture_size_t;

typedef enum
{
    GPU_VERTEX_POSTIION = 1 << 0,
    GPU_VERTEX_UV       = 1 << 1,
    GPU_VERTEX_COLOR    = 1 << 2,
} gpu_vertex_attribute_t;

typedef enum
{
    GPU_PRIMITIVE_TRIANGLES = 0,
    GPU_PRIMITIVE_LINES     = 1,
    GPU_PRIMITIVE_POINTS    = 2,
} gpu_primitive_t;

typedef enum
{
    GPU_TRANSFORM_MODEL      = 0,
    GPU_TRANSFORM_VIEW       = 1,
    GPU_TRANSFORM_PROJECTION = 2,
} gpu_transform_t;

typedef struct
{
    nu_u32_t max_vertex_count;
    nu_u32_t max_texture_counts[4];
} gpu_config_t;

typedef struct
{
    nu_u32_t texture;
    nu_v4u_t scissor;
    nu_v4u_t viewport;
    nu_m4_t  model;
    nu_m4_t  view;
    nu_m4_t  projection;
} gpu_state_t;

typedef struct
{
    nu_bool_t          active;
    gpu_texture_size_t size;
} gpu_texture_t;

typedef struct
{
    nu_bool_t              active;
    nu_u32_t               count;
    gpu_primitive_t        primitive;
    gpu_vertex_attribute_t attributes;
} gpu_mesh_t;

typedef struct
{
    nu_u32_t mesh;
    nu_u32_t texture;
    nu_m4_t  transform;
} gpu_node_t;

typedef struct
{
    gpu_state_t   state;
    gpu_config_t  config;
    gpu_texture_t textures[GPU_MAX_TEXTURE];
    gpu_mesh_t    meshes[GPU_MAX_MESH];
    gpu_node_t    nodes[GPU_MAX_NODE];
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
    CART_CHUNK_MESH    = 3,
    CART_CHUNK_MODEL   = 4,
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
        nu_u32_t               index;
        nu_u16_t               count;
        gpu_primitive_t        primitive;
        gpu_vertex_attribute_t flags;
    } mesh;
    struct
    {
        nu_u32_t node_first;
        nu_u32_t node_count;
    } model;
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

#define VM_CONFIG_DEFAULT                                                 \
    (vm_config_t)                                                         \
    {                                                                     \
        .cpu.mem_heap_size = NU_MEM_1M, .cpu.mem_stack_size = NU_MEM_64K, \
        .gpu.max_vertex_count = 1024,                                     \
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
