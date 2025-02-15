#ifndef NUX_PRINT_H
#define NUX_PRINT_H

#include <nulib/nulib.h>

void nux_println(nu_char_t *fmt, ...);

#ifdef NUX_IMPLEMENTATION
#include "print_impl.h"
#endif

#endif
