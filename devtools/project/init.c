#include "project.h"

#include "templates_data.h"

void
nux_command_init (nu_sv_t path, nu_sv_t lang)
{
    printf("init package " NU_SV_FMT " " NU_SV_FMT "\n",
           NU_SV_ARGS(path),
           NU_SV_ARGS(lang));

    nux_project_t project;

    // Find lang
    nux_project_template_file_t *template_file = NU_NULL;
    if (nu_sv_eq(lang, NU_SV("c")))
    {
        template_file = template_c_files;
        nux_project_init(&project, path, 1);
        project.entries[0].header.type   = NUX_CHUNK_WASM;
        project.entries[0].header.length = 0;
        nu_sv_to_cstr(NU_SV("build/cart.wasm"),
                      project.entries[0].source_path,
                      NU_PATH_MAX);
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

    // Save nux project file
    nux_project_save(&project, path);
    nux_project_free(&project);
}
