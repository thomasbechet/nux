#ifndef NUX_CORE_PLATFORM_H
#define NUX_CORE_PLATFORM_H

#include <core/core.h>

NUX_API void *nux_os_alloc(void *p, nux_u32_t osize, nux_u32_t nsize);
NUX_API void  nux_os_log(nux_log_level_t level,
                         const nux_c8_t *log,
                         nux_u32_t       len);
NUX_API void  nux_os_stats_update(nux_u64_t *stats);

NUX_API nux_status_t nux_os_file_open(nux_u32_t       slot,
                                      const nux_c8_t *path,
                                      nux_u32_t       len,
                                      nux_io_mode_t   mode);
NUX_API void         nux_os_file_close(nux_u32_t slot);
NUX_API nux_status_t nux_os_file_stat(nux_u32_t slot, nux_file_stat_t *stat);
NUX_API nux_status_t nux_os_file_seek(nux_u32_t slot, nux_u32_t cursor);
NUX_API nux_u32_t    nux_os_file_read(nux_u32_t slot, void *p, nux_u32_t n);
NUX_API nux_u32_t nux_os_file_write(nux_u32_t slot, const void *p, nux_u32_t n);
NUX_API void      nux_os_hotreload_add(const nux_c8_t *path, nux_rid_t handle);
NUX_API void      nux_os_hotreload_remove(nux_rid_t handle);
NUX_API void      nux_os_hotreload_pull(nux_rid_t *handles, nux_u32_t *count);

#endif
