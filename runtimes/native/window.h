#ifndef WINDOW_H
#define WINDOW_H

#include <nulib/nulib.h>

nu_status_t window_init(void);
void        window_free(void);
void        window_poll_events(void);
void        window_swap_buffers(void);
nu_bool_t   window_close_requested(void);
nu_b2i_t    window_get_render_viewport(void);

#endif
