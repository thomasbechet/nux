#include "project.h"

void
nux_command_init (nu_sv_t path, nu_sv_t lang)
{
    printf("init package " NU_SV_FMT " " NU_SV_FMT "\n",
           NU_SV_ARGS(path),
           NU_SV_ARGS(lang));

    nux_project_t pkg;
    nux_project_load(&pkg, path);
    // nu_sv_to_cstr(name, pkg.name, NUX_NAME_MAX);
    // nu_sv_to_cstr(NU_SV("test"), pkg.target_path, NU_PATH_MAX);
    // pkg.entries     = NU_NULL;
    // pkg.entry_count = 0;

    nux_project_save(&pkg, NU_SV("."));
    nux_project_free(&pkg);

    // Find lang
    static struct
    {
        const nu_sv_t name;
    } languages[] = {
        NU_SV("c"),
    };
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(languages); i++)
    {
        if (nu_sv_eq(languages[i].name, lang))
        {
        }
    }
}
