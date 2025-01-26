#include "cli.h"

#include <argparse/argparse.h>
#include <native/runtime.h>
#ifdef NUX_BUILD_SDK
#include <sdk.h>
#endif

nu_u32_t
cli_command_run (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux run [-h] [path]",
        NULL,
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc         = argparse_parse(&argparse, argc, argv);
    nu_sv_t path = NU_SV(".");
    if (argc >= 1)
    {
        path = nu_sv_cstr(argv[0]);
    }
    nu_status_t status = NU_SUCCESS;
#ifdef NUX_BUILD_SDK
    if (nu_isdir(path))
    {
        sdk_project_t project;
        if (!sdk_project_load(&project, path))
        {
            return -1;
        }
        path                = nu_sv_cstr(project.target_path);
        runtime_info_t info = {
            path,
            cli_log,
        };
        status = runtime_init(&info);
        sdk_project_free(&project);
        return status ? 0 : -1;
    }
#endif
    runtime_info_t info = {
        path,
        cli_log,
    };
    status = runtime_init(&info);
    return status ? 0 : -1;
}
