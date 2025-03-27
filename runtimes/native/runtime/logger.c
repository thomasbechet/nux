#include "runtime.h"

static runtime_log_callback_t logger;

void
logger_set_callback (runtime_log_callback_t callback)
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
nux_platform_log (nux_instance_t inst, const nux_c8_t *log, nux_u32_t n)
{
    logger_log(NU_LOG_INFO, "%*.s", n, log);
}
