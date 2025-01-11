#include "commands.h"

#include <argparse/argparse.h>
#include <project/project.h>
#include <vmnative/runtime.h>

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
    if (nu_isdir(path))
    {
        project_t project;
        NU_ASSERT(project_load(&project, path));
        path = nu_sv_cstr(project.target_path);
        vmn_run(path);
        project_free(&project);
    }
    else
    {
        vmn_run(path);
    }
    return 0;
}
