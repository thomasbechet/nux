#include "cli.h"

#ifdef NUX_BUILD_SDK

#include <argparse/argparse.h>
#include <sdk.h>

nu_u32_t
cli_command_dump (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux dump [-h] [-v] [-s] [-t] [-n] <cart>",
        NULL,
    };
    nu_bool_t              sort          = NU_FALSE;
    nu_bool_t              display_table = NU_FALSE;
    nu_u32_t               num           = 0;
    struct argparse_option options[]     = {
        OPT_HELP(),
        OPT_BOOLEAN('s', "sort", &sort, "sort entries by usage", NU_NULL, 0, 0),
        OPT_BOOLEAN(
            't', "table", &display_table, "display chunk table", NU_NULL, 0, 0),
        OPT_INTEGER('n',
                    "num",
                    &num,
                    "display the first n entries from table",
                    NU_NULL,
                    0,
                    0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 1)
    {
        argparse_usage(&argparse);
        return -1;
    }
    nu_status_t status
        = sdk_dump(nu_sv_cstr(argv[0]), sort, display_table, num);
    return status ? 0 : -1;
}

#endif
