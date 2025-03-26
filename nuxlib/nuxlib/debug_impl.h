#include "debug.h"

#include <nux.h>

void
nux_debug (void)
{
    char buf[256];
    push_cursor(10, 10);
    nu_snprintf(buf, sizeof(buf), "time: %.2lf", global_time());
    print(buf);
    nu_f32_t memusage = (nu_f32_t)console_info(CONSOLE_MEMORY_USAGE)
                        / (nu_f32_t)console_info(CONSOLE_MEMORY_CAPACITY);
    nu_snprintf(buf, sizeof(buf), "mem : %.2lf%%", memusage * 100);
    print(buf);
    nu_snprintf(buf, sizeof(buf), "res : %ux%u", SCREEN_WIDTH, SCREEN_HEIGHT);
    print(buf);
}
