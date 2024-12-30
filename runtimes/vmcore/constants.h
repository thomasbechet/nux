#ifndef NUX_CONSTANTS_H
#define NUX_CONSTANTS_H

typedef enum
{
    NUX_CHUNK_RAW    = 0,
    NUX_CHUNK_WASM   = 1,
    NUX_CHUNK_TEX64  = 2,
    NUX_CHUNK_TEX128 = 3,
    NUX_CHUNK_TEX256 = 4,
} nux_chunk_type_t;

#endif
