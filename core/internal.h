#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#define NU_STDLIB
#include <nulib/nulib.h>
#ifdef NUX_BUILD_WASM3
#include <wasm3.h>
#endif

struct nux_env
{
    nux_instance_t inst;
    nu_v2u_t       cursor;
    nux_error_t    error;
    nux_c8_t       error_message[256];

    nu_v3_t   positions[4];
    nu_v2_t   uvs[4];
    nux_u32_t vertex_index;
};

struct nux_instance
{
    void *userdata;

    nu_bool_t running;
    nux_f32_t time;
    nux_u32_t tps;

    nux_u8_t *memory;
    nux_u32_t memory_capa;

    nu_u32_t buttons[NUX_PLAYER_MAX];
    nu_f32_t axis[NUX_PLAYER_MAX][NUX_AXIS_MAX];

    struct nux_env env;

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
