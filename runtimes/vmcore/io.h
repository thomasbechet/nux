#ifndef NU_IO_H
#define NU_IO_H

#include <nulib.h>

typedef struct
{
    nu_u32_t version;
    nu_u32_t chunk_count;
} nux_cart_header_t;

typedef enum
{
    NUX_CHUNK_WASM,
    NUX_CHUNK_TEX64,
    NUX_CHUNK_TEX128,
    NUX_CHUNK_TEX256,
    NUX_CHUNK_MESH,
    NUX_CHUNK_RAW,
} nux_cart_chunk_type_t;

typedef struct
{
    nux_cart_chunk_type_t type;
    nu_u32_t              offset;
    nu_u32_t              length;
    nu_u32_t              dst;
} nux_cart_chunk_t;

typedef struct
{
    nu_u64_t          handle;
    nux_cart_header_t header;
} nux_cart_t;

nu_u32_t nux_cart_load_header(const nu_byte_t   *name,
                              void              *user,
                              nux_cart_header_t *header);
nu_u32_t nux_cart_load_full(nux_cart_t *cart, nu_u32_t id);
nu_u32_t nux_cart_load_dest(nux_cart_t *cart, nu_u32_t id, nu_u32_t dst);
nu_u32_t nux_cart_load(nux_cart_t *cart, nu_u32_t id);

#endif
