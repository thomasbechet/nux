#include <vmnative/runtime.h>
#include <builder/builder.h>
#include <argparse/argparse.h>

struct cmd_struct
{
    nu_str_t cmd;
    int (*fn)(int, const char **);
};

static int
cmd_run (int argc, const char **argv)
{
    const char       *path = NULL;
    struct argparse   argparse;
    const char *const usages[] = {
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
static int
cmd_init (int argc, const char **argv)
{
    return 0;
}
static int
cmd_build (int argc, const char **argv)
{
    struct argparse   argparse;
    const char *const usages[] = {
        "nux build [-h] <path>",
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
    const nu_byte_t *path = (const nu_byte_t *)argv[0];
    nux_build_cart(path);
    return 0;
}

int
main (int argc, const char *argv[])
{
    nu_bool_t         version = NU_FALSE;
    struct argparse   argparse;
    const char *const usages[] = {
        "nux [-h] [-v] <command> [<args>]",
        NULL,
    };
    struct cmd_struct commands[] = {
        { NU_STR("run"), cmd_run },
        { NU_STR("init"), cmd_init },
        { NU_STR("build"), cmd_build },
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
        printf("nux version 1.0.0\n");
        return 0;
    }
    else if (argc < 1)
    {
        argparse_usage(&argparse);
        return -1;
    }

    struct cmd_struct *cmd      = NULL;
    nu_str_t           cmd_name = nu_str_from_cstr((nu_byte_t *)argv[0]);
    for (int i = 0; i < NU_ARRAY_SIZE(commands); i++)
    {
        if (nu_str_eq(commands[i].cmd, cmd_name))
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
