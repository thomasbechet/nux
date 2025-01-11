#ifndef VM_TYPES_H
#define VM_TYPES_H

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
} vm_gpu_state_t;

typedef struct
{
    vm_gpu_state_t state;
    nu_u8_t       *cmds;
} vm_gpu_t;

// IO

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} vm_cart_header_t;

typedef struct
{
    vm_cart_header_t header;
    void            *heap;
} vm_io_t;

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
} vm_wasm_t;

// VM

typedef struct vm
{
    vm_config_t config;
    vm_gpu_t    gpu;
    vm_wasm_t   wasm;
    vm_io_t     io;
    void       *heap;
    nu_size_t   heap_size;
    void       *heap_ptr;
    void       *user;
} vm_t;

#endif
