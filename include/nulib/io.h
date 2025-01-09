#ifndef NU_IO_H
#define NU_IO_H

#include <nulib/types.h>
#include <nulib/string.h>

NU_API nu_bool_t nu_load_bytes(nu_sv_t    filename,
                               nu_byte_t *data,
                               nu_size_t *size);
NU_API nu_bool_t nu_save_bytes(nu_sv_t          filename,
                               const nu_byte_t *data,
                               nu_size_t        size);
NU_API nu_bool_t nu_isdir(nu_sv_t path);

#endif
