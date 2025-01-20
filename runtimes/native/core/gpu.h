#ifndef GPU_H
#define GPU_H

#include "vm.h"

nu_status_t gpu_init(vm_t *vm);
nu_status_t gpu_free(vm_t *vm);
void        gpu_render(vm_t *vm);

// Pools
void gpu_init_pool(vm_t *vm, nu_u32_t pool_index, nu_u32_t mem_size);
void gpu_free_pool(vm_t *vm, nu_u32_t pool_index);

// Textures
void gpu_alloc_texture(vm_t       *vm,
                       nu_u32_t    texture_index,
                       nu_u32_t    pool_index,
                       nu_u32_t    w,
                       nu_u32_t    h,
                       const void *p);
void gpu_write_texture(vm_t       *vm,
                       nu_u32_t    texture_index,
                       nu_u32_t    x,
                       nu_u32_t    y,
                       nu_u32_t    w,
                       nu_u32_t    h,
                       const void *p);

// Meshes
void gpu_alloc_mesh(vm_t       *vm,
                    nu_u32_t    mesh_index,
                    nu_u32_t    pool_index,
                    nu_u32_t    count,
                    const void *p);
void gpu_write_mesh(vm_t       *vm,
                    nu_u32_t    mesh_index,
                    nu_u32_t    first,
                    nu_u32_t    count,
                    const void *p);

// Commands
void gpu_draw(vm_t *vm, nu_u32_t mesh_index, nu_u32_t texture_index);

#endif
