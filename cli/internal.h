#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "cli.h"

#include <argparse/argparse.h>
#ifdef NUX_BUILD_SDK
#include <sdk.h>
#endif
#include <runtime.h>

void cli_log(nu_log_level_t level, const nu_char_t *fmt, va_list args);

#endif
