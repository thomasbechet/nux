#ifndef NUX_PACKAGE_H
#define NUX_PACKAGE_H

#include <nulib.h>
#include <vmcore/config.h>

#define NUX_NAME_MAX 256

typedef struct
{
    nux_chunk_header_t header;
    nu_char_t          source[NU_PATH_MAX];
} nux_chunk_entry_t;

typedef struct
{
    nu_char_t          name[NUX_NAME_MAX];
    nu_char_t          target[NUX_NAME_MAX];
    nux_chunk_entry_t *entries;
    nu_size_t          entry_count;
} nux_package_t;

NU_API nu_byte_t *load_bytes(nu_sv_t filename, nu_size_t *size);
NU_API void nux_package_init(nu_sv_t package_file, nux_package_t *package);
NU_API void nux_package_free(nux_package_t *package);
NU_API void nux_init_package(nu_sv_t path);
NU_API void nux_build_package(nu_sv_t path);

#endif
