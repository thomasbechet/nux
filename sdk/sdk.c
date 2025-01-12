#include "sdk.h"

static sdk_log_callback_t log_callback;

void
sdk_set_log_callback (sdk_log_callback_t callback)
{
    log_callback = callback;
}

void
sdk_vlog (nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    if (log_callback)
    {
        log_callback(level, fmt, args);
    }
}
void
sdk_log (nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    sdk_vlog(level, fmt, args);
    va_end(args);
}
