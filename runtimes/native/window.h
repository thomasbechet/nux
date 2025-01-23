#ifndef WINDOW_H
#define WINDOW_H

#include <nulib/nulib.h>
#include "core/vm.h"

#define WINDOW_WIDTH  VM_SCREEN_WIDTH
#define WINDOW_HEIGHT VM_SCREEN_HEIGHT

nu_status_t window_init(void);
void        window_free(void);
void        window_poll_events(void);
void        window_swap_buffers(void);
nu_bool_t   window_close_requested(void);

#endif
