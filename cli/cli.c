#include "cli.h"

#include <argparse/argparse.h>
#include <core/vm.h>
#include <runtime.h>
#ifdef NUX_BUILD_SDK
#include <sdk.h>
#endif

typedef struct
{
    nu_sv_t cmd;
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
        "nux [-h] [-v] <command> [<args>]",
        NULL,
    };
    cmd_entry_t commands[] = {
#ifdef NUX_BUILD_SDK
        { NU_SV("init"), cli_command_init },
        { NU_SV("build"), cli_command_build },
        { NU_SV("dump"), cli_command_dump },
        { NU_SV("editor"), cli_command_editor },
#endif
        { NU_SV("run"), cli_command_run },
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
#ifdef NUX_BUILD_SDK
                      "\n    init    Create a new nux project"
                      "\n    build   Compile a project to cartridge"
                      "\n    dump    Print information about cartridge"
                      "\n    editor  Launch project editor"
#endif
                      "\n    run     Run a cartridge");
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
            VM_VERSION_MAJOR(VM_VERSION),
            VM_VERSION_MINOR(VM_VERSION),
            VM_VERSION_PATCH(VM_VERSION));
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
