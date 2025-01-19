#include "logger.h"

#include "../platform.h"

static runtime_log_callback_t logger;

void
logger_init (runtime_log_callback_t callback)
{
    logger = callback;
}
void
logger_log (nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    logger_vlog(level, fmt, args);
    va_end(args);
}
void
logger_vlog (nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    logger(level, fmt, args);
}

void
os_vlog (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    logger_vlog(level, fmt, args);
}
