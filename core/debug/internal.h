#ifndef NUX_DEBUG_INTERNAL_H
#define NUX_DEBUG_INTERNAL_H

#include <io/api.h>
#include <ecs/api.h>
#include <base/internal.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef struct nux_debug_module
{
    nux_rid_t console_canvas;
    nux_c8_t *lines;
    nux_u32_t lines_count;
    nux_u32_t lines_cursor;
} nux_debug_module_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// debug.c

nux_status_t nux_debug_init(nux_ctx_t *ctx);
void         nux_debug_free(nux_ctx_t *ctx);
void         nux_debug_update(nux_ctx_t *ctx);
void         nux_debug_log_callback(nux_ctx_t      *ctx,
                                    nux_log_level_t level,
                                    const nux_c8_t *buf,
                                    nux_u32_t       n);

#endif
