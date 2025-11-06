#ifndef NUX_DEBUG_INTERNAL_H
#define NUX_DEBUG_INTERNAL_H

#include <debug/api.h>

void nux_debug_module_register(void);

void nux_debug_log_callback(nux_log_level_t level,
                            const nux_c8_t *buf,
                            nux_u32_t       n);

#endif
