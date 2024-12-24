#ifndef NU_GPU_H
#define NU_GPU_H

#include <nulib.h>
#include <wasm_export.h>

typedef struct
{
    nu_u32_t texture;
    nu_u32_t buffer;
    nu_v4u_t scissor;
    nu_v4u_t viewport;
    nu_m4_t  model;
    nu_u32_t mode;
    nu_u8_t *cmds;
} nux_gpu_state_t;

typedef struct
{
    nu_u32_t max_texs;
    nu_u32_t max_texm;
    nu_u32_t max_texl;
    nu_u32_t max_texh;
    nu_u32_t max_vert;
} nux_gpu_config_t;

typedef struct
{
    nux_gpu_config_t config;
    nux_gpu_state_t  state;
    nu_u8_t         *cmds;
} nux_gpu_t;

void nux_gpu_init(nux_gpu_t *gpu);

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
