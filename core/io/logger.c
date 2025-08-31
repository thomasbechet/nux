#include "internal.h"

#include <debug/internal.h>

void
nux_vlog (nux_ctx_t      *ctx,
          nux_log_level_t level,
          const nux_c8_t *fmt,
          va_list         args)
{
    if (level <= ctx->config.log.level)
    {
        nux_c8_t  buf[256];
        nux_u32_t n = nux_vsnprintf(buf, sizeof(buf), fmt, args);
        nux_os_log(ctx->userdata, level, buf, n);
        nux_debug_log_callback(ctx, level, buf, n);
    }
}
void
nux_log (nux_ctx_t *ctx, nux_log_level_t level, const nux_c8_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nux_vlog(ctx, level, fmt, args);
    va_end(args);
}

void
nux_log_set_level (nux_ctx_t *ctx, nux_log_level_t level)
{
    ctx->config.log.level = level;
}
