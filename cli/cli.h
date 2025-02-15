#ifndef CLI_H
#define CLI_H

#include <nulib/nulib.h>

NU_API void cli_log(nu_log_level_t level, const nu_char_t *fmt, va_list args);

#ifdef NUX_BUILD_SDK
NU_API nu_u32_t cli_command_init(nu_u32_t argc, const nu_char_t **argv);
NU_API nu_u32_t cli_command_build(nu_u32_t argc, const nu_char_t **argv);
NU_API nu_u32_t cli_command_dump(nu_u32_t argc, const nu_char_t **argv);
#endif
NU_API nu_u32_t cli_command_run(nu_u32_t argc, const nu_char_t **argv);

#endif
