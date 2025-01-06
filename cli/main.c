#include <vmcore/vm.h>
#include <vmnative/runtime.h>
#include <builder/builder.h>
#include <argparse/argparse.h>

typedef struct
{
    nu_sv_t cmd;
    nu_u32_t (*fn)(nu_u32_t, const nu_char_t **);
} cmd_entry_t;

static nu_u32_t
cmd_run (nu_u32_t argc, const nu_char_t **argv)
{
    const nu_char_t       *path = NULL;
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux run [-h] <cart>",
        NULL,
    };
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
    nux_runtime_run(argc, argv);
    return 0;
}
static nu_u32_t
cmd_init (nu_u32_t argc, const nu_char_t **argv)
{
    return 0;
}
static nu_u32_t
cmd_build (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux build [-h] <workdir>",
        NULL,
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc            = argparse_parse(&argparse, argc, argv);
    nu_sv_t workdir = NU_SV(".");
    if (argc >= 1)
    {
        workdir = nu_sv_cstr(argv[0]);
    }
    nux_build_project(workdir);
    return 0;
}

int
main (int argc, const nu_char_t *argv[])
{
    nu_bool_t              version = NU_FALSE;
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux [-h] [-v] <command> [<args>]",
        NULL,
    };
    cmd_entry_t commands[] = {
        { NU_SV("run"), cmd_run },
        { NU_SV("init"), cmd_init },
        { NU_SV("build"), cmd_build },
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", &version, "show version", NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);
    argparse_describe(&argparse,
                      "\nNUX is fantasy retro console",
                      "\nCOMMANDS\n"
                      "\n    run     Execute a cartridge"
                      "\n    init    Create a new nux project"
                      "\n    build   Compile project to cartridge");
    argc = argparse_parse(&argparse, argc, argv);
    if (version)
    {
        printf("nux runtime " NUX_RUNTIME_VERSION "\n");
        return 0;
    }
    else if (argc < 1)
    {
        argparse_usage(&argparse);
        return -1;
    }

    cmd_entry_t *cmd      = NULL;
    nu_sv_t      cmd_name = nu_sv_cstr(argv[0]);
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(commands); i++)
    {
        if (nu_sv_eq(commands[i].cmd, cmd_name))
        {
            cmd = &commands[i];
        }
    }
    if (!cmd)
    {
        argparse_usage(&argparse);
        return -1;
    }
    return cmd->fn(argc, argv);
}
