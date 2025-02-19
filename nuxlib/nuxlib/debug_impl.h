#include "debug.h"

#include <nux.h>

void
nux_debug (void)
{
    char buf[256];
    cursor(100, 10);
    sprintf(buf, "t: %.2lf", time());
    nu_f32_t memusage = (nu_f32_t)console_info(CONSOLE_MEMORY_USAGE)
                        / (nu_f32_t)console_info(CONSOLE_MEMORY_CAPACITY);
    sprintf(buf, "m: %.2lf%%", memusage * 100);
    print(buf);
}
