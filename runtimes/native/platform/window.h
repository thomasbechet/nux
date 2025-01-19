#ifndef WINDOW_H
#define WINDOW_H

#include <nulib.h>

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500

nu_status_t window_init(void);
void        window_free(void);
void        window_poll_events(void);
void        window_swap_buffers(void);
nu_bool_t   window_close_requested(void);

#endif
