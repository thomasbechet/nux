#ifndef VMN_WINDOW_H
#define VMN_WINDOW_H

#include <vmcore/vm.h>

#define VMN_WINDOW_WIDTH  500
#define VMN_WINDOW_HEIGHT 500

nu_status_t vmn_window_init(void);
void        vmn_window_free(void);
void        vmn_window_poll_events(void);
void        vmn_window_swap_buffers(void);
nu_bool_t   vmn_window_close_requested(void);

#endif
