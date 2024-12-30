#ifndef NUX_WINDOW_H
#define NUX_WINDOW_H

#include "error.h"

#include <vm.h>

#define NUX_WINDOW_WIDTH  500
#define NUX_WINDOW_HEIGHT 500
#define NUX_SCREEN_WIDTH  640
#define NUX_SCREEN_HEIGHT 400

nux_error_code_t nux_window_init(void);
nux_error_code_t nux_window_free(void);
void             nux_window_poll_events(void);
void             nux_window_swap_buffers(void);
nu_bool_t        nux_window_close_requested(void);

#endif
