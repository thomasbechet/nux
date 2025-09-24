#ifndef NUX_DEBUG_MODULE_H
#define NUX_DEBUG_MODULE_H

#include <io/module.h>
#include <ecs/module.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// debug.c

void nux_debug_log_callback(nux_log_level_t level,
                            const nux_c8_t *buf,
                            nux_u32_t       n);

#endif
