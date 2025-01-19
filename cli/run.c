#include "commands.h"

#include <argparse/argparse.h>
#include <native/vm.h>
#include <sdk.h>

static nu_status_t
run (nu_sv_t path)
{
    nu_status_t status;
    vm_t        vm;
    vm_config_t config = VM_CONFIG_DEFAULT;

    status = vm_init(&vm, &config, &vm);
    NU_CHECK(status, return NU_FAILURE);

    nu_char_t name[NU_PATH_MAX];
    nu_sv_to_cstr(path, name, NU_PATH_MAX);
    status = vm_load(&vm, name);
    NU_CHECK(status, goto cleanup0);

    for (;;)
    {
    }

cleanup0:
    vm_free(&vm);

    return status;
}

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
    if (nu_isdir(path))
    {
        sdk_project_t project;
        if (!sdk_project_load(&project, path))
        {
            return -1;
        }
        path   = nu_sv_cstr(project.target_path);
        status = run(path);
        sdk_project_free(&project);
    }
    else
    {
        status = run(path);
    }
    return status ? 0 : -1;
}
