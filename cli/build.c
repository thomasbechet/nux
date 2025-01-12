#include "commands.h"

#include <vmcore/config.h>
#include <argparse/argparse.h>
#include <project/project.h>
#include <parson/parson.h>

nu_u32_t
cli_command_build (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux build [-h] [-p <path>] [-v]",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    nu_bool_t              verbose   = NU_FALSE;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('p', "path", &path, "project location", NU_NULL, 0, 0),
        OPT_BOOLEAN(
            'v', "verbose", &verbose, "show useful info", NU_NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc            = argparse_parse(&argparse, argc, argv);
    nu_sv_t path_sv = NU_SV(".");
    if (path)
    {
        path_sv = nu_sv_cstr(path);
    }
    project_t       project;
    project_error_t error;
    nu_status_t     status;
    status = project_load(&project, path_sv, &error);
    NU_CHECK(status, goto cleanup0);
    status = project_build(&project, &error);
    NU_CHECK(status, goto cleanup1);
cleanup1:
    project_free(&project);
cleanup0:
    if (!status)
    {
        project_error_print(&error);
        return -1;
    }
    return 0;
}
