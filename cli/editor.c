#include "cli.h"

#ifdef NUX_BUILD_SDK

#include <argparse/argparse.h>
#include <sdk.h>

nu_u32_t
cli_command_editor (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux editor [-h] [-p <path>] [-v]",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('p', "path", &path, "project location", NU_NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc            = argparse_parse(&argparse, argc, argv);
    nu_sv_t path_sv = NU_SV(".");
    if (path)
    {
        path_sv = nu_sv(path, NU_PATH_MAX);
    }
    runtime_config_t config
        = { .views_count = 0, .debug = NU_FALSE, .path = path_sv };
    nu_status_t status = runtime_start(&config);
    return status ? 0 : -1;
}

#endif
