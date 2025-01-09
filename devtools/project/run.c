#include "project.h"

#include <vmnative/runtime.h>

void
nux_command_run (nu_sv_t path)
{
    if (nu_isdir(path))
    {
        nux_project_t project;
        NU_ASSERT(nux_project_load(&project, path));
        path = nu_sv_cstr(project.target_path);
        nux_runtime_run(path);
        nux_project_free(&project);
    }
    else
    {
        nux_runtime_run(path);
    }
}
