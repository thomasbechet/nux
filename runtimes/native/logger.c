#include "internal.h"

void
logger_log (nux_log_level_t level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    logger_vlog(level, fmt, args);
    va_end(args);
}
void
logger_vlog (nux_log_level_t level, const char *fmt, va_list args)
{
    switch (level)
    {
        case NUX_LOG_DEBUG:
            fprintf(stdout, "\x1B[36mDEBUG\x1B[0m ");
            break;
        case NUX_LOG_INFO:
            fprintf(stdout, "\x1B[32mINFO\x1B[0m ");
            break;
        case NUX_LOG_WARNING:
            fprintf(stdout, "\033[0;33mWARN ");
            break;
        case NUX_LOG_ERROR:
            fprintf(stdout, "\x1B[31mERROR ");
            break;
    }
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\x1B[0m\n");
}
