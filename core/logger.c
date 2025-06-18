#include "internal.h"

void
nux_vlog (nux_env_t      *env,
          nux_log_level_t level,
          const nux_c8_t *fmt,
          va_list         args)
{
    nux_c8_t  buf[256];
    nux_u32_t n = nux_vsnprintf(buf, sizeof(buf), fmt, args);
    nux_os_console(env->inst->userdata, level, buf, n);
}
void
nux_log (nux_env_t *env, nux_log_level_t level, const nux_c8_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nux_vlog(env, level, fmt, args);
    va_end(args);
}
