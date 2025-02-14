#ifndef GPU_H
#define GPU_H

#include "syscall.h"
#include "cartridge.h"

/////////////////
///    GPU    ///
/////////////////

// #define GPU_SCREEN_WIDTH  640
// #define GPU_SCREEN_HEIGHT 400

#define GPU_SCREEN_WIDTH  512
#define GPU_SCREEN_HEIGHT 320

// #define GPU_SCREEN_WIDTH  480
// #define GPU_SCREEN_HEIGHT 300

// #define GPU_SCREEN_WIDTH  360
// #define GPU_SCREEN_HEIGHT 225

// #define GPU_SCREEN_WIDTH  320
// #define GPU_SCREEN_HEIGHT 200

#define GPU_MAX_POOL        32
#define GPU_MAX_TEXTURE     1024
#define GPU_MAX_MESH        1024
#define GPU_MAX_MODEL       1024
#define GPU_MAX_SPRITESHEET 256

#define GPU_GLOBAL_POOL (gpu_index_t)(-1)

#define GPU_MIN_TEXTURE_SIZE 32
#define GPU_MAX_TEXTURE_SIZE 512
#define GPU_ADDR_NULL        (gpu_addr_t)(-1)

typedef nu_i32_t gpu_index_t;
typedef nu_i32_t gpu_addr_t;

typedef struct
{
    nu_u32_t vram_capacity;
} gpu_config_t;

typedef struct
{
    nu_v4u_t    scissor;
    nu_v4u_t    viewport;
    nu_m4_t     model;
    nu_m4_t     view;
    nu_m4_t     projection;
    nu_v2u_t    cursor;
    gpu_index_t pool;
    nu_color_t  fog_color;
    nu_f32_t    fog_density;
    nu_f32_t    fog_near;
    nu_f32_t    fog_far;
    nu_color_t  color;
} gpu_state_t;

typedef struct
{
    nu_u32_t   size;
    nu_u32_t   capa;
    gpu_addr_t addr;
} gpu_pool_t;

typedef struct
{
    nu_u32_t    size;
    gpu_index_t pool;
    gpu_addr_t  addr;
} gpu_texture_t;

typedef struct
{
    gpu_index_t            pool;
    gpu_addr_t             addr;
    nu_u32_t               count;
    sys_primitive_t        primitive;
    sys_vertex_attribute_t attributes;
} gpu_mesh_t;

typedef struct
{
    gpu_index_t pool;
    gpu_addr_t  addr;
    nu_u32_t    node_count;
} gpu_model_t;

typedef struct
{
    gpu_index_t texture;
    gpu_index_t mesh;
    nu_m4_t     local_to_parent;
    gpu_index_t parent;
} gpu_model_node_t;

typedef struct
{
    gpu_index_t texture;
    nu_u32_t    row;
    nu_u32_t    col;
    nu_u32_t    fwidth;
    nu_u32_t    fheight;
} gpu_spritesheet_t;

typedef struct
{
    nu_byte_t        *vram;
    nu_u32_t          vram_size;
    nu_u32_t          vram_capa;
    gpu_state_t       state;
    gpu_pool_t        pools[GPU_MAX_POOL];
    gpu_texture_t     textures[GPU_MAX_TEXTURE];
    gpu_mesh_t        meshes[GPU_MAX_MESH];
    gpu_model_t       models[GPU_MAX_MODEL];
    gpu_spritesheet_t spritesheets[GPU_MAX_SPRITESHEET];
} gpu_t;

nu_status_t gpu_init(vm_t *vm, const gpu_config_t *config);
nu_status_t gpu_free(vm_t *vm);
void        gpu_reload_state(vm_t *vm);
void        gpu_begin_frame(vm_t *vm);
void        gpu_end_frame(vm_t *vm);
gpu_addr_t  gpu_malloc(vm_t *vm, nu_u32_t n);

nu_u32_t gpu_texture_memsize(nu_u32_t size);
nu_u32_t gpu_vertex_memsize(sys_vertex_attribute_t attributes, nu_u32_t count);
nu_u32_t gpu_vertex_offset(sys_vertex_attribute_t attributes,
                           sys_vertex_attribute_t attribute,
                           nu_u32_t               count);

#endif
