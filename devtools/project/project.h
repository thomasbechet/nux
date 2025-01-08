#ifndef NUX_PROJECT_H
#define NUX_PROJECT_H

#include <nulib.h>
#include <vmcore/config.h>

#define NUX_NAME_MAX 256

typedef struct
{
    nux_chunk_header_t header;
    nu_char_t          source_path[NU_PATH_MAX];
} nux_chunk_entry_t;

typedef struct
{
    nu_char_t          name[NUX_NAME_MAX];
    nu_char_t          target_path[NU_PATH_MAX];
    nux_chunk_entry_t *entries;
    nu_size_t          entry_count;
} nux_project_t;

NU_API nu_byte_t *load_bytes(nu_sv_t filename, nu_size_t *size);
NU_API nu_bool_t  nux_project_load(nux_project_t *package, nu_sv_t path);
NU_API nu_bool_t  nux_project_save(const nux_project_t *package, nu_sv_t path);
NU_API void       nux_project_free(nux_project_t *package);

NU_API void nux_command_init(nu_sv_t path, nu_sv_t name, nu_sv_t lang);
NU_API void nux_command_build(nu_sv_t path);
NU_API void nux_command_run(nu_sv_t path);

#endif
