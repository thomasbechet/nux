#ifndef WINDOW_H
#define WINDOW_H

#include <nulib/nulib.h>

typedef enum
{
    COMMAND_EXIT,
    COMMAND_SAVE_STATE,
    COMMAND_LOAD_STATE,
} window_command_t;

struct RGFW_window;
typedef struct RGFW_window RGFW_window;

nu_status_t  window_init(void);
void         window_free(void);
RGFW_window *window_get_win(void);
void         window_poll_events(void);
void         window_swap_buffers(void);
nu_b2i_t     window_get_render_viewport(void);
nu_v2u_t     window_get_size(void);
nu_bool_t    window_poll_command(window_command_t *cmd);

#endif
