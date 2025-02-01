#ifndef GPU_H
#define GPU_H

#include "shared.h"
#include "cartridge.h"

/////////////////
///    GPU    ///
/////////////////

// #define GPU_SCREEN_WIDTH  640
// #define GPU_SCREEN_HEIGHT 400
#define GPU_SCREEN_WIDTH  512
#define GPU_SCREEN_HEIGHT 320
// #define GPU_SCREEN_HEIGHT 448
// #define GPU_SCREEN_WIDTH  320
// #define GPU_SCREEN_HEIGHT 200

#define GPU_MAX_POOL    32
#define GPU_MAX_TEXTURE 1024
#define GPU_MAX_MESH    1024
#define GPU_MAX_MODEL   1024

#define GPU_GLOBAL_POOL (nu_u32_t)(-1)

#define GPU_MIN_TEXTURE_SIZE 32
#define GPU_MAX_TEXTURE_SIZE 512

typedef enum
{
    GPU_VERTEX_POSITION = 1 << 0,
    GPU_VERTEX_UV       = 1 << 1,
    GPU_VERTEX_COLOR    = 1 << 2,
    GPU_VERTEX_INDICES  = 1 << 3,
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
    nu_u32_t vram_capacity;
} gpu_config_t;

typedef struct
{
    nu_v4u_t scissor;
    nu_v4u_t viewport;
    nu_m4_t  model;
    nu_m4_t  view;
    nu_m4_t  projection;
    nu_u32_t pool;
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
    nu_u32_t  size;
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
    nu_bool_t active;
    nu_u32_t  node_count;
} gpu_model_t;

typedef struct
{
    nu_u32_t texture;
    nu_u32_t mesh;
    nu_m4_t  local_to_parent;
    nu_u32_t parent;
} gpu_model_node_t;

typedef struct
{
    nu_u32_t      vram_remaining;
    gpu_state_t   state;
    gpu_config_t  config;
    gpu_pool_t    pools[GPU_MAX_POOL];
    gpu_texture_t textures[GPU_MAX_TEXTURE];
    gpu_mesh_t    meshes[GPU_MAX_MESH];
    gpu_model_t   models[GPU_MAX_MODEL];
} gpu_t;

nu_status_t gpu_init(vm_t *vm, const gpu_config_t *config);
nu_status_t gpu_free(vm_t *vm);
void        gpu_begin(vm_t *vm);
void        gpu_end(vm_t *vm);

nu_status_t gpu_load_texture(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t gpu_load_mesh(vm_t *vm, const cart_chunk_header_t *header);
nu_status_t gpu_load_model(vm_t *vm, const cart_chunk_header_t *header);

nu_status_t gpu_alloc_pool(vm_t *vm, nu_u32_t index, nu_u32_t size);
nu_status_t gpu_bind_pool(vm_t *vm, nu_u32_t index);
nu_status_t gpu_clear_pool(vm_t *vm, nu_u32_t index);

nu_status_t gpu_alloc_texture(vm_t *vm, nu_u32_t index, nu_u32_t size);
nu_status_t gpu_update_texture(vm_t       *vm,
                               nu_u32_t    index,
                               nu_u32_t    x,
                               nu_u32_t    y,
                               nu_u32_t    w,
                               nu_u32_t    h,
                               const void *p);

nu_status_t gpu_alloc_mesh(vm_t                  *vm,
                           nu_u32_t               index,
                           nu_u32_t               count,
                           gpu_primitive_t        primitive,
                           gpu_vertex_attribute_t attributes);
nu_status_t gpu_update_mesh(vm_t                  *vm,
                            nu_u32_t               index,
                            gpu_vertex_attribute_t attributes,
                            nu_u32_t               first,
                            nu_u32_t               count,
                            const void            *p);

nu_status_t gpu_alloc_model(vm_t *vm, nu_u32_t index, nu_u32_t node_count);
nu_status_t gpu_update_model(vm_t           *vm,
                             nu_u32_t        index,
                             nu_u32_t        node_index,
                             nu_u32_t        mesh,
                             nu_u32_t        texture,
                             nu_u32_t        parent,
                             const nu_f32_t *transform);

void gpu_push_transform(vm_t *vm, gpu_transform_t transform, const nu_f32_t *m);
void gpu_draw_model(vm_t *vm, nu_u32_t index);
void gpu_draw_text(vm_t *vm, nu_u32_t x, nu_u32_t y, const void *text);

nu_u32_t gpu_vertex_memsize(gpu_vertex_attribute_t attributes, nu_u32_t count);
nu_u32_t gpu_vertex_size(gpu_vertex_attribute_t attributes);
nu_u32_t gpu_vertex_offset(gpu_vertex_attribute_t attributes,
                           gpu_vertex_attribute_t attribute);
nu_u32_t gpu_texture_memsize(nu_u32_t size);

#endif
