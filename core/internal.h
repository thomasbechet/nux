#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#define NU_STDLIB
#include <nulib/nulib.h>
#ifdef NUX_BUILD_WASM3
#include <wasm3.h>
#endif

#define NUX_MEMPTR(inst, addr, type) ((type *)((inst)->memory + (addr)))
#define NUX_MEMGET(inst, addr, type) *(const type *)((inst)->memory + (addr))
#define NUX_MEMSET(inst, addr, type, val) \
    *(type *)((inst)->memory + (addr)) = (val)

#define NUX_RGB(p) \
    (nux_u32_t)(p)[0] << 16 | (nux_u32_t)(p)[1] << 8 | (nux_u32_t)(p)[2];
#define NUX_WRITE_RGB(p, rgb)        \
    (p)[0] = (rgb & 0xFF0000) >> 16; \
    (p)[1] = (rgb & 0x00FF00) >> 8;  \
    (p)[2] = (rgb & 0x0000FF) >> 0;

struct nux_env
{
    // Non persistent state
    nux_instance_t inst;

    // Error handling
    nux_error_t error;
    nux_c8_t    error_message[256];

    // Stats
    nux_u32_t tricount;

    // Draw state
    nux_u8_t          *target_color;
    nu_v2u_t           target_color_size;
    const nux_u8_t    *texture_data;
    nu_v2u_t           texture_size;
    nux_texture_type_t texture_type;
};

struct nux_instance
{
    void     *userdata;
    nu_bool_t running;

    nux_u8_t *memory;
    nux_u32_t memory_capa;

    struct nux_env env;
    nux_callback_t init;
    nux_callback_t update;

    struct
    {
        nu_bool_t started;
        nu_u8_t  *buffer;
        nu_u32_t  buffer_size;
#ifdef NUX_BUILD_WASM3
        IM3Environment env;
        IM3Runtime     runtime;
        IM3Module      module;
        IM3Function    start_function;
        IM3Function    update_function;
#endif
    } wasm;
};

void nux_set_error(nux_env_t env, nux_error_t error);

nux_status_t nux_wasm_init(nux_instance_t               inst,
                           const nux_instance_config_t *config);
void         nux_wasm_free(nux_instance_t inst);
nux_status_t nux_wasm_load(nux_env_t env, nux_u8_t *buffer, nux_u32_t n);
nux_status_t nux_wasm_start(nux_env_t env);
nux_status_t nux_wasm_update(nux_env_t env);

#endif
