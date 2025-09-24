#ifndef NUX_DEBUG_INTERNAL_H
#define NUX_DEBUG_INTERNAL_H

#include "module.h"

typedef struct
{
    nux_canvas_t *console_canvas;
    nux_c8_t     *lines;
    nux_u32_t     lines_count;
    nux_u32_t     lines_cursor;
} nux_debug_module_t;

nux_debug_module_t *nux_debug_module(void);

nux_status_t nux_debug_init(void);
void         nux_debug_free(void);
void         nux_debug_update(void);

#endif
