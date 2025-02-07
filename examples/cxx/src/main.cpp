#include <string>
#include <cstdarg>

#include "nux.h"

void
println (const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[256];
    int  n = std::vsnprintf(buf, sizeof(buf), fmt, args);
    trace(buf, n);
    va_end(args);
}

void
start (void)
{
    println("hello world");
}
void
update (void)
{
}
