#ifndef NUX_BASE_PLATFORM_H
#define NUX_BASE_PLATFORM_H

#include <base/api.h>

NUX_API void *nux_os_alloc(void     *userdata,
                           void     *p,
                           nux_u32_t osize,
                           nux_u32_t nsize);
NUX_API void  nux_os_log(void           *userdata,
                         nux_log_level_t level,
                         const nux_c8_t *log,
                         nux_u32_t       len);
NUX_API void  nux_os_stats_update(void *userdata, nux_u64_t *stats);

#endif
