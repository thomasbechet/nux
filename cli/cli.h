#ifndef CLI_H
#define CLI_H

#define NU_STDLIB
#include <nulib/nulib.h>

#ifdef NUX_BUILD_SDK
NU_API nu_u32_t cli_command_init(nu_u32_t argc, const nu_char_t **argv);
NU_API nu_u32_t cli_command_build(nu_u32_t argc, const nu_char_t **argv);
NU_API nu_u32_t cli_command_dump(nu_u32_t argc, const nu_char_t **argv);
#endif
NU_API nu_u32_t cli_command_run(nu_u32_t argc, const nu_char_t **argv);

#endif
