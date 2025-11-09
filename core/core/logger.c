#include "internal.h"

#include <debug/internal.h>

void
nux_vlog (nux_log_level_t level, const nux_c8_t *fmt, va_list args)
{
    if (level <= nux_log_level())
    {
        nux_c8_t  buf[256];
        nux_u32_t n = nux_vsnprintf(buf, sizeof(buf), fmt, args);
        nux_os_log(level, buf, n);
        nux_debug_log_callback(level, buf, n);
    }
}
void
nux_log (nux_log_level_t level, const nux_c8_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nux_vlog(level, fmt, args);
    va_end(args);
}

void
nux_log_set_level (nux_log_level_t level)
{
    nux_config()->log.level = level;
}
nux_log_level_t
nux_log_level (void)
{
    return nux_config()->log.level;
}
