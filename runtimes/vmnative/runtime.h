#ifndef VMN_H
#define VMN_H

#include <nulib.h>

typedef void (*vmn_log_callback_t)(nu_log_level_t   level,
                                   const nu_char_t *fmt,
                                   va_list          args);

NU_API nu_status_t vmn_execute(nu_sv_t path);
NU_API void        vmn_set_log_callback(vmn_log_callback_t callback);

void vmn_vlog(nu_log_level_t level, const nu_char_t *fmt, va_list args);
void vmn_log(nu_log_level_t level, const nu_char_t *fmt, ...);

#endif
