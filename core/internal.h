#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#define NU_STDLIB
#include <nulib/nulib.h>
#ifdef NUX_BUILD_WASM3
#include <wasm3.h>
#endif

#define NUX_MEMPTR(inst, addr, type) ((type *)((inst)->state + (addr)))
#define NUX_MEMGET(inst, addr, type) *(const type *)((inst)->state + (addr))
#define NUX_MEMSET(inst, addr, type, val) \
    *(type *)((inst)->state + (addr)) = (val)

struct nux_env
{
    // Non persistent state
    nux_instance_t inst;

    // Error handling
    nux_error_t error;
    nux_c8_t    error_message[256];

    // Stats
    nux_u32_t tricount;
};

struct nux_instance
{
    void     *userdata;
    nu_bool_t running;

    nux_u8_t *state;

    nux_f32_t *gpu_buffer;
    nux_u32_t  gpu_buffer_size;

    nux_gpu_command_t *gpu_commands;
    nux_u32_t          gpu_commands_size;

    struct nux_env env;
    nux_callback_t init;
    nux_callback_t update;
};

void nux_set_error(nux_env_t env, nux_error_t error);

nux_status_t nux_wasm_init(nux_instance_t               inst,
                           const nux_instance_config_t *config);
void         nux_wasm_free(nux_instance_t inst);
nux_status_t nux_wasm_load(nux_env_t env, nux_u8_t *buffer, nux_u32_t n);
nux_status_t nux_wasm_start(nux_env_t env);
nux_status_t nux_wasm_update(nux_env_t env);

nux_u32_t          nux_push_gpu_data(nux_env_t        env,
                                     const nux_f32_t *data,
                                     nux_u32_t        count);
nux_gpu_command_t *nux_push_gpu_command(nux_env_t env);

#endif
