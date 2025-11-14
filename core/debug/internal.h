#ifndef nux_debug_INTERNAL_H
#define nux_debug_INTERNAL_H

#include <debug/debug.h>

void nux_debug_module_register(void);

void nux_debug_log_callback(nux_log_level_t level,
                            const nux_c8_t *buf,
                            nux_u32_t       n);

#endif
