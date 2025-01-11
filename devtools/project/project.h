#ifndef PROJECT_H
#define PROJECT_H

#include <nulib.h>
#include <vmcore/config.h>

#define PROJECT_NAME_MAX 256

typedef struct
{
    vm_chunk_header_t header;
    nu_char_t         source_path[NU_PATH_MAX];
} project_chunk_entry_t;

typedef struct
{
    nu_char_t              target_path[NU_PATH_MAX];
    project_chunk_entry_t *entries;
    nu_size_t              entries_count;
    nu_char_t              prebuild[PROJECT_NAME_MAX];
} project_t;

NU_API nu_bool_t project_generate_template(nu_sv_t path, nu_sv_t lang);
NU_API nu_bool_t project_build(const project_t *project);
NU_API nu_bool_t project_load(project_t *project, nu_sv_t path);
NU_API nu_bool_t project_save(const project_t *project, nu_sv_t path);
NU_API void      project_free(project_t *project);

#endif
