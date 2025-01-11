#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include <nulib.h>

NU_API nu_u32_t cli_command_init(nu_u32_t argc, const nu_char_t **argv);
NU_API nu_u32_t cli_command_build(nu_u32_t argc, const nu_char_t **argv);
NU_API nu_u32_t cli_command_run(nu_u32_t argc, const nu_char_t **argv);

#endif
