#include "internal.h"

#define LOG_LINE_LEN   64
#define LOG_LINE_COUNT 25

static struct
{
    nux_canvas_t *console_canvas;
    nux_c8_t     *lines;
    nux_u32_t     lines_count;
    nux_u32_t     lines_cursor;
} _module;

static void
module_update (void)
{
    // Render logs
    // nux_u32_t y = 100;
    // for (nux_u32_t i = 0; i < _module.lines_count; ++i)
    // {
    //     nux_canvas_text(
    //         _module.console_canvas, 10, y, _module.lines + LOG_LINE_LEN * i);
    //     y += 10;
    // }
}
static nux_status_t
module_init (void)
{
    // Register systems
    nux_system_register(NUX_SYSTEM_UPDATE, module_update);

    // Create debug console
    if (nux_config_get()->debug.console)
    {
        // Create canvas
        _module.console_canvas = nux_canvas_new(nux_arena_core(), 1000, 500);
        NUX_CHECK(_module.console_canvas, return NUX_FAILURE);
    }

    // Create log buffer
    _module.lines
        = nux_arena_malloc(nux_arena_core(), LOG_LINE_LEN * LOG_LINE_COUNT);
    NUX_CHECK(_module.lines, return NUX_FAILURE);
    _module.lines_count  = 0;
    _module.lines_cursor = 0;

    return NUX_SUCCESS;
}
void
nux_debug_module_register (void)
{
    NUX_MODULE_REGISTER("debug", &_module, module_init, nullptr);
}

void
nux_debug_log_callback (nux_log_level_t level, const nux_c8_t *buf, nux_u32_t n)
{
    if (_module.lines) // ignore logs before module initialization
    {
        nux_strncpy(_module.lines + LOG_LINE_LEN * _module.lines_cursor,
                    buf,
                    LOG_LINE_LEN);
        _module.lines_count = NUX_MIN(LOG_LINE_COUNT, _module.lines_count + 1);

        ++_module.lines_cursor;
        _module.lines_cursor = _module.lines_cursor % LOG_LINE_COUNT;
    }
}
