#include "project.h"

#include <cJSON/cJSON.h>

void
nux_command_init (nu_sv_t path, nu_sv_t name, nu_sv_t lang)
{
    printf("init package " NU_SV_FMT " " NU_SV_FMT " " NU_SV_FMT "\n",
           NU_SV_ARGS(path),
           NU_SV_ARGS(name),
           NU_SV_ARGS(lang));

    nux_project_t pkg;
    nu_sv_to_cstr(name, pkg.name, NUX_NAME_MAX);

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
