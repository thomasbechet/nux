#ifndef NU_WINDOW_H
#define NU_WINDOW_H

#include <vm.h>

#define NU_WINDOW_WIDTH  500
#define NU_WINDOW_HEIGHT 500

void      nu_window_init(void);
void      nu_window_free(void);
void      nu_window_poll_events(void);
nu_bool_t nu_window_close_requested(void);

#endif
