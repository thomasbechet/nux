#include "project.h"

#include "templates_data.h"

void
nux_command_init (nu_sv_t path, nu_sv_t lang, nu_bool_t verbose)
{
    if (verbose)
    {
        printf("Initialize new project " NU_SV_FMT " with language " NU_SV_FMT
               "\n",
               NU_SV_ARGS(path),
               NU_SV_ARGS(lang));
    }

    // Find lang
    nux_project_template_file_t *template_file = NU_NULL;
    if (nu_sv_eq(lang, NU_SV("c")))
    {
        template_file = template_c_files;
    }

    // Template found, generate files
    if (template_file)
    {
        while (template_file->path)
        {
            nu_char_t filepath[NU_PATH_MAX];
            nu_sv_t   filepath_sv = nu_path_concat(
                filepath, NU_PATH_MAX, path, nu_sv_cstr(template_file->path));
            NU_ASSERT(nu_save_bytes(
                filepath_sv, template_file->data, template_file->size));
            ++template_file;
        }
    }
    else
    {
        // Generate empty project file
        nux_project_t project;
        nux_project_init_empty(&project, path);
        nux_project_save(&project, path);
        nux_project_free(&project);
    }
}
