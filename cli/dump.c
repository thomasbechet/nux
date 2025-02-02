#include "cli.h"

#ifdef NUX_BUILD_SDK

#include <argparse/argparse.h>
#include <sdk.h>

nu_u32_t
cli_command_dump (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux dump [-h] [-v] <cart>",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 1)
    {
        argparse_usage(&argparse);
        return -1;
    }
    nu_status_t status = sdk_dump(nu_sv_cstr(argv[0]));
    return status ? 0 : -1;
}

#endif
