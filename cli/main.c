#include "cli.h"

#include <argparse/argparse.h>
#include <sdk.h>

typedef struct
{
    nu_sv_t cmd;
    nu_u32_t (*fn)(nu_u32_t, const nu_char_t **);
} cmd_entry_t;

void
cli_log (nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    switch (level)
    {
        case NU_LOG_DEBUG:
            fprintf(stdout, "\x1B[36mDEBUG\x1B[0m ");
            break;
        case NU_LOG_INFO:
            fprintf(stdout, "\x1B[32mINFO\x1B[0m ");
            break;
        case NU_LOG_WARNING:
            fprintf(stdout, "\033[0;33mWARN\x1B[0m ");
            break;
        case NU_LOG_ERROR:
            fprintf(stdout, "\x1B[31mERROR\x1B[0m ");
            break;
    }
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
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
        { NU_SV("init"), cli_command_init },
        { NU_SV("build"), cli_command_build },
        { NU_SV("run"), cli_command_run },
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", &version, "show version", NULL, 0, 0),
        OPT_END(),
    };

    sdk_set_log_callback(cli_log);

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
        printf("nux runtime " VM_RUNTIME_VERSION "\n");
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
