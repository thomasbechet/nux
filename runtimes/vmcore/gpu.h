#ifndef NUX_GPU_H
#define NUX_GPU_H

#include "types.h"

void nux_gpu_init(nux_vm_t *vm);

void write_texture(wasm_exec_env_t env,
                   nu_u32_t        type,
                   nu_u32_t        slot,
                   const void     *p);
void write_vertex(wasm_exec_env_t env,
                  nu_u32_t        first,
                  nu_u32_t        count,
                  const void     *p);
void bind_texture(wasm_exec_env_t env, nu_u32_t type, nu_u32_t slot);
void draw(wasm_exec_env_t env, nu_u32_t first, nu_u32_t count);

#endif
