#ifndef NUX_TYPES_H
#define NUX_TYPES_H

#include <nulib.h>
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

// IO
//

typedef enum
{
    NUX_CHUNK_RAW  = 0,
    NUX_CHUNK_WASM = 1,
    NUX_CHUNK_TEXS = 2,
    NUX_CHUNK_TEXM = 3,
    NUX_CHUNK_TEXL = 4,
    NUX_CHUNK_MESH = 5,
} nux_chunk_type_t;

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} nux_cart_header_t;

typedef struct
{
    nux_chunk_type_t type;
    nu_u32_t         length;
    nu_u32_t         dst; // slot, addr...
} nux_chunk_header_t;

typedef struct
{
    nux_cart_header_t header;
} nux_cart_t;

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

typedef struct
{
    void     *runtime_heap;
    nu_size_t runtime_heap_size;
} nux_wasm_info_t;

// VM

typedef struct nux_vm_t
{
    nux_gpu_t  gpu;
    nux_wasm_t wasm;
    nux_cart_t cart;
    void      *heap_ptr;
    nu_size_t  heap_size;
    void      *heap;
    void      *user;
    nu_u32_t   cart_id;
} nux_vm_t;

#endif
