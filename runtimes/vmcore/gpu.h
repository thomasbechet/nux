#ifndef VM_GPU_H
#define VN_GPU_H

#include "types.h"

void write_texture(wasm_exec_env_t env,
                   nu_u32_t        slot,
                   nu_u32_t        x,
                   nu_u32_t        y,
                   nu_u32_t        w,
                   nu_u32_t        h,
                   const void     *p);
void write_vertex(wasm_exec_env_t env,
                  nu_u32_t        first,
                  nu_u32_t        count,
                  const void     *p);
void bind_texture(wasm_exec_env_t env, nu_u32_t slot);
void draw(wasm_exec_env_t env, nu_u32_t first, nu_u32_t count);

#endif
