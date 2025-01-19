#include "logger.h"

#include "../platform.h"

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
    switch (level)
    {
        case NU_LOG_DEBUG:
            fprintf(stdout, "\x1B[36mDEBUG\x1B[0m ");
            break;
        case NU_LOG_INFO:
            fprintf(stdout, "\x1B[32mINFO\x1B[0m ");
            break;
        case NU_LOG_WARNING:
            fprintf(stdout, "\033[0;33mWARN\x1B[0m ");
            break;
        case NU_LOG_ERROR:
            fprintf(stdout, "\x1B[31mERROR\x1B[0m ");
            break;
    }
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

void
os_vlog (void *user, nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    logger_vlog(level, fmt, args);
}
