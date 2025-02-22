#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "syscall.h"

#define CART_HEADER_SIZE      sizeof(cart_header_t)
#define CART_CHUNK_ENTRY_SIZE (sizeof(nu_u32_t) * 4)

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
    nu_u32_t          id;
    nu_u32_t          offset;
    nu_u32_t          length;
} cart_chunk_entry_t;

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} cart_header_t;

nu_size_t   cart_read(vm_t *vm, void *p, nu_size_t n);
nu_status_t cart_read_u32(vm_t *vm, nu_u32_t *v);
nu_status_t cart_read_f32(vm_t *vm, nu_f32_t *v);
nu_status_t cart_read_m4(vm_t *vm, nu_m4_t *v);

nu_status_t cart_parse_header(const void *data, cart_header_t *header);
nu_status_t cart_parse_entries(const void         *data,
                               nu_u32_t            count,
                               cart_chunk_entry_t *entries);

#endif
