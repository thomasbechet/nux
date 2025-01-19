#ifndef VM_WINDOW_H
#define VM_WINDOW_H

#include "../vm.h"

#define VM_WINDOW_WIDTH  500
#define VM_WINDOW_HEIGHT 500

nu_status_t window_init(void);
void        window_free(void);
void        window_poll_events(void);
void        window_swap_buffers(void);
nu_bool_t   window_close_requested(void);

#endif
