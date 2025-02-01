#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "shared.h"

#define CART_CHUNK_MESH_HEADER_SIZE sizeof(nu_u32_t) * 4

typedef enum
{
    CART_CHUNK_RAW     = 0,
    CART_CHUNK_WASM    = 1,
    CART_CHUNK_TEXTURE = 2,
    CART_CHUNK_MESH    = 3,
    CART_CHUNK_MODEL   = 4,
} cart_chunk_type_t;

typedef struct
{
    cart_chunk_type_t type;
    nu_u32_t          length;
} cart_chunk_header_t;

typedef struct
{
    nu_u32_t version;
} cart_header_t;

nu_size_t   cart_read(vm_t *vm, void *p, nu_size_t n);
nu_status_t cart_read_u32(vm_t *vm, nu_u32_t *v);
nu_status_t cart_read_f32(vm_t *vm, nu_f32_t *v);
nu_status_t cart_read_m4(vm_t *vm, nu_m4_t *v);

#endif
