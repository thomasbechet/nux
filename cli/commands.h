#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include <nulib.h>

NU_API void cli_command_init(nu_sv_t path, nu_sv_t lang, nu_bool_t verbose);
NU_API void cli_command_build(nu_sv_t path, nu_bool_t verbose);
NU_API void cli_command_run(nu_sv_t path);

#endif
