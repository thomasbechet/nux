#ifndef NUX_TYPES_H
#define NUX_TYPES_H

#include "config.h"

#include <wasm_export.h>

// GPU

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
    nux_gpu_state_t state;
    nu_u8_t        *cmds;
} nux_gpu_t;

// IO

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} nux_cart_header_t;

typedef struct
{
    nux_cart_header_t header;
    void             *heap;
} nux_io_t;

// WASM

typedef struct
{
    void                *buffer;
    nu_size_t            buffer_size;
    wasm_module_t        module;
    wasm_module_inst_t   instance;
    wasm_exec_env_t      env;
    wasm_function_inst_t start_callback;
    wasm_function_inst_t update_callback;
} nux_wasm_t;

// VM

typedef struct nux_vm_t
{
    nux_vm_config_t config;
    nux_gpu_t       gpu;
    nux_wasm_t      wasm;
    nux_io_t        io;
    void           *heap;
    nu_size_t       heap_size;
    void           *heap_ptr;
    void           *user;
} nux_vm_t;

#endif
