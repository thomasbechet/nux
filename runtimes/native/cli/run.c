#include "cli.h"

#include <argparse/argparse.h>
#include <runtime/runtime.h>
#ifdef NUX_BUILD_SDK
#include <sdk/sdk.h>
#endif

nu_u32_t
cli_command_run (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux [-h] "
#ifdef NUX_BUILD_WASM_DEBUG
        "[-d] "
#endif
        "[path]",
        NULL,
    };
    nu_bool_t              debug     = NU_FALSE;
    struct argparse_option options[] = {
        OPT_HELP(),
#ifdef NUX_BUILD_WASM_DEBUG
        OPT_BOOLEAN(
            'd', "debug", &debug, "start with debug server", NU_NULL, 0, 0),
#endif
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc           = argparse_parse(&argparse, argc, argv);
    nu_sv_t   path = argc >= 1 ? nu_sv(argv[0], NU_PATH_MAX) : nu_sv_empty();
    nu_char_t target_path[NU_PATH_MAX];
#ifdef NUX_BUILD_SDK
    if (!path.len)
    {
        path = NU_SV(".");
    }
    if (nu_path_isdir(path))
    {
        sdk_project_t project;
        if (!sdk_project_load(&project, path))
        {
            return -1;
        }
        nu_strncpy(target_path, project.target_path, NU_PATH_MAX);
        path = nu_sv(target_path, NU_PATH_MAX);
        sdk_project_free(&project);
    }
#endif
    runtime_config_t config = { .debug = debug, .path = path };
    nu_status_t      status = runtime_run(&config);
    return status ? 0 : -1;
}
