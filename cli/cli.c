#include "internal.h"

typedef struct
{
    const nu_char_t *cmd;
    nu_u32_t (*fn)(nu_u32_t, const nu_char_t **);
} cmd_entry_t;

static nu_log_level_t log_level;

void
cli_log (nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    if (level > log_level)
    {
        return;
    }
    switch (level)
    {
        case NU_LOG_DEBUG:
            fprintf(stdout, "\x1B[36mDEBUG\x1B[0m ");
            break;
        case NU_LOG_INFO:
            fprintf(stdout, "\x1B[32mINFO\x1B[0m ");
            break;
        case NU_LOG_WARNING:
            fprintf(stdout, "\033[0;33mWARN ");
            break;
        case NU_LOG_ERROR:
            fprintf(stdout, "\x1B[31mERROR ");
            break;
    }
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\x1B[0m\n");
}

int
main (int argc, const nu_char_t *argv[])
{
    nu_bool_t              version = NU_FALSE;
    struct argparse        argparse;
    const nu_char_t *const usages[] = {
        "nux [-h] [-v] [command | cartridge] [<args>]",
        NULL,
    };
    cmd_entry_t commands[] = {
        { "run", cli_command_run },
#ifdef NUX_BUILD_SDK
        { "init", cli_command_init },
        { "build", cli_command_build },
        { "dump", cli_command_dump },
#endif
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", &version, "show version", NULL, 0, 0),
        OPT_END(),
    };

    log_level = NU_LOG_INFO;
    logger_set_callback(cli_log);

    argparse_init(&argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);
    argparse_describe(&argparse,
                      "\nNUX is fantasy retro console",
                      "\nCOMMANDS\n"
                      "\n    run     Start console"
#ifdef NUX_BUILD_SDK
                      "\n    init    Create a new nux project"
                      "\n    build   Compile a project to cartridge"
                      "\n    dump    Print information about cartridge"
#endif
    );
    argc = argparse_parse(&argparse, argc, argv);
    if (version)
    {
        printf(
            "nux runtime v%d.%d.%d ("
#ifdef NUX_BUILD_SDK
            "with sdk, "
#else
            "no sdk, "
#endif
#ifdef NUX_BUILD_WASM_DEBUG
            "debug interpreter"
#elif NUX_BUILD_WASM_JIT
            "jit interpreter"
#else
            "default interpreter"
#endif
            ")\n",
            NUX_VERSION_MAJOR(NUX_VERSION),
            NUX_VERSION_MINOR(NUX_VERSION),
            NUX_VERSION_PATCH(NUX_VERSION));
        return 0;
    }

    if (argc > 0)
    {
        // Try to find subcommand
        cmd_entry_t *cmd = NULL;
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(commands); i++)
        {
            if (nu_strneq(commands[i].cmd, argv[0], NU_PATH_MAX))
            {
                cmd = &commands[i];
                break;
            }
        }
        if (cmd) // Subcommand found
        {
            return cmd->fn(argc, argv);
        }
    }

    // No subcommand found, execute run by default

    nu_sv_t path = argc > 0 ? nu_sv(argv[0], NU_PATH_MAX) : nu_sv_empty();
    runtime_config_t config = { .debug = NU_FALSE, .path = path };
    return runtime_run(&config) ? 0 : -1;
}
