#include <project/project.h>

#include <argparse/argparse.h>

typedef struct
{
    nu_sv_t cmd;
    nu_u32_t (*fn)(nu_u32_t, const nu_char_t **);
} cmd_entry_t;

static nu_u32_t
cmd_init (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux init [-h] [-p <path>] [-l <lang>]",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    const nu_char_t       *lang      = NU_NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING(
            'p', "path", &path, "init project at location", NU_NULL, 0, 0),
        OPT_STRING(
            'l', "lang", &lang, "initialize the wasm language", NU_NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc = argparse_parse(&argparse, argc, argv);
    nux_command_init(path ? nu_sv_cstr(path) : NU_SV("."),
                     lang ? nu_sv_cstr(lang) : nu_sv_null());
    return 0;
}
static nu_u32_t
cmd_build (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux build [-h] [-p <path>]",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('p', "path", &path, "project location", NU_NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc            = argparse_parse(&argparse, argc, argv);
    nu_sv_t path_sv = NU_SV(".");
    if (path)
    {
        path_sv = nu_sv_cstr(path);
    }
    nux_command_build(path_sv);
    return 0;
}
static nu_u32_t
cmd_run (nu_u32_t argc, const nu_char_t **argv)
{
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux run [-h] [-p <path>] [cart]",
        NULL,
    };
    const nu_char_t       *path      = NU_NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('p', "path", &path, "location of project", NU_NULL, 0, 0),
        OPT_END(),
    };
    argparse_init(&argparse, options, usages, 0);
    argc            = argparse_parse(&argparse, argc, argv);
    nu_sv_t path_sv = NU_SV(".");
    if (path)
    {
        path_sv = nu_sv_cstr(path);
    }
    nu_sv_t cart_sv = nu_sv_null();
    if (argc >= 1)
    {
        cart_sv = nu_sv_cstr(argv[1]);
    }
    nux_command_run(path_sv, cart_sv);
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
        { NU_SV("init"), cmd_init },
        { NU_SV("build"), cmd_build },
        { NU_SV("run"), cmd_run },
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
                      "\n    init    Create a new nux project"
                      "\n    build   Compile a project to cartridge"
                      "\n    run     Run a cartridge");
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
