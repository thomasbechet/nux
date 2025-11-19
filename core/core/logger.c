#include "internal.h"

#include <debug/internal.h>

void
nux_logger_vlog (nux_log_level_t level, const nux_c8_t *fmt, va_list args)
{
    if (level <= nux_logger_level())
    {
        nux_c8_t  buf[256];
        nux_u32_t n = nux_vsnprintf(buf, sizeof(buf), fmt, args);
        nux_os_log(level, buf, n);
    }
}
void
nux_logger_log (nux_log_level_t level, const nux_c8_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nux_logger_vlog(level, fmt, args);
    va_end(args);
}

void
nux_logger_set_level (nux_log_level_t level)
{
    nux_config_get()->log.level = level;
}
nux_log_level_t
nux_logger_level (void)
{
    return nux_config_get()->log.level;
}
