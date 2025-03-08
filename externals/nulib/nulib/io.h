#ifndef NU_IO_H
#define NU_IO_H

#include "string.h"
#include "types.h"

NU_API nu_status_t nu_load_bytes(nu_sv_t    filename,
                                 nu_byte_t *data,
                                 nu_size_t *size);
NU_API nu_status_t nu_save_bytes(nu_sv_t          filename,
                                 const nu_byte_t *data,
                                 nu_size_t        size);
NU_API nu_bool_t   nu_isdir(nu_sv_t path);
NU_API nu_size_t   nu_list_files(nu_sv_t path,
                                 nu_char_t (*files)[NU_PATH_MAX],
                                 nu_size_t capa);

#endif
