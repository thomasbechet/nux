#ifndef VM_LOGGER_H
#define VM_LOGGER_H

#include <nulib.h>

void logger_log(nu_log_level_t level, const nu_char_t *fmt, ...);
void logger_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);
void logger_set_callback(void);

#endif
