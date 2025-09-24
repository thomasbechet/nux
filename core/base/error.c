#include "internal.h"

void
nux_error (const nux_c8_t *fmt, ...)
{
    nux_base_module_t *module = nux_base_module();
    if (module->error_enable)
    {
        va_list args;
        va_start(args, fmt);
        nux_vsnprintf(
            module->error_message, sizeof(module->error_message), fmt, args);
        va_end(args);
#ifdef NUX_BUILD_DEBUG
        NUX_ERROR("%s", nux_error_get_message());
        NUX_ASSERT(NUX_FALSE);
#endif
        module->error_status = NUX_FAILURE;
    }
}
void
nux_error_enable (void)
{
    nux_base_module()->error_enable = NUX_TRUE;
}
void
nux_error_disable (void)
{
    nux_base_module()->error_enable = NUX_FALSE;
}
void
nux_error_reset (void)
{
    nux_base_module_t *module = nux_base_module();
    module->error_status      = NUX_SUCCESS;
    nux_memset(module->error_message, 0, sizeof(module->error_message));
}
const nux_c8_t *
nux_error_get_message (void)
{
    return nux_base_module()->error_message;
}
nux_status_t
nux_error_get_status (void)
{
    return nux_base_module()->error_status;
}
