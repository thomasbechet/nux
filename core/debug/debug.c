#include "internal.h"

#include <base/internal.h>

#define LOG_LINE_LEN   64
#define LOG_LINE_COUNT 25

nux_status_t
nux_debug_init (void)
{
    // Create module
    nux_debug_module_t *module = nux_debug_module();

    // Create debug console
    if (nux_config()->debug.console)
    {
        // Create canvas
        module->console_canvas = nux_canvas_new(nux_arena_core(), 1000, 500);
        NUX_CHECK(module->console_canvas, return NUX_FAILURE);
    }

    // Create log buffer
    module->lines
        = nux_arena_malloc(nux_arena_core(), LOG_LINE_LEN * LOG_LINE_COUNT);
    NUX_CHECK(module->lines, return NUX_FAILURE);
    module->lines_count  = 0;
    module->lines_cursor = 0;

    return NUX_SUCCESS;
}
void
nux_debug_free (void)
{
}
void
nux_debug_update (void)
{
    nux_debug_module_t *module = nux_debug_module();

    // Render logs
    // nux_u32_t y = 100;
    // for (nux_u32_t i = 0; i < module->lines_count; ++i)
    // {
    //     nux_canvas_text(
    //         module->console_canvas, 10, y, module->lines + LOG_LINE_LEN * i);
    //     y += 10;
    // }
}
void
nux_debug_log_callback (nux_log_level_t level, const nux_c8_t *buf, nux_u32_t n)
{
    nux_debug_module_t *module = nux_debug_module();
    if (module && module->lines) // ignore logs before module initialization
    {
        nux_strncpy(module->lines + LOG_LINE_LEN * module->lines_cursor,
                    buf,
                    LOG_LINE_LEN);
        module->lines_count = NUX_MIN(LOG_LINE_COUNT, module->lines_count + 1);

        ++module->lines_cursor;
        module->lines_cursor = module->lines_cursor % LOG_LINE_COUNT;
    }
}
