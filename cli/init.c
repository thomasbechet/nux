#include "commands.h"

#include <argparse/argparse.h>
#include <project/project.h>

nu_u32_t
cli_command_init (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux init [-h] [-p <path>] [-l <lang>]",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    const nu_char_t       *lang      = NU_NULL;
    nu_bool_t              verbose   = NU_FALSE;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING(
            'p', "path", &path, "init project at location", NU_NULL, 0, 0),
        OPT_STRING(
            'l', "lang", &lang, "initialize the wasm language", NU_NULL, 0, 0),
        OPT_BOOLEAN(
            'v', "verbose", &verbose, "show useful info", NU_NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc = argparse_parse(&argparse, argc, argv);
    project_error_t error;
    if (!project_generate_template(path ? nu_sv_cstr(path) : NU_SV("."),
                                   lang ? nu_sv_cstr(lang) : nu_sv_null(),
                                   &error))
    {
        project_error_print(&error);
        return -1;
    }
    return 0;
}
