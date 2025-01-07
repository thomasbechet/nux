#include "package.h"

#include <cJSON/cJSON.h>

void
nux_command_init (nu_sv_t path, nu_sv_t name, nu_sv_t lang)
{
    printf("init package " NU_SV_FMT " " NU_SV_FMT " " NU_SV_FMT "\n",
           NU_SV_ARGS(path),
           NU_SV_ARGS(name),
           NU_SV_ARGS(lang));

    // Generate project file
    cJSON *jroot = cJSON_CreateObject();
    if (jroot == NULL)
    {
        goto end;
    }

    name = cJSON_CreateString("Awesome 4K");
    if (name == NULL)
    {
        goto end;
    }

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

cleanup0:;
}
