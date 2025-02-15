#include "print.h"

#include <nux.h>

void
nux_println (nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    nu_char_t buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    trace(buf);
    va_end(args);
}
