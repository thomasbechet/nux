#ifndef LOGGER_H
#define LOGGER_H

#include "../runtime.h"

void logger_init(runtime_log_callback_t callback);
void logger_log(nu_log_level_t level, const nu_char_t *fmt, ...);
void logger_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);

#endif
