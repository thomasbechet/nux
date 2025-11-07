#ifndef NUX_CORE_PLATFORM_H
#define NUX_CORE_PLATFORM_H

#include <core/core.h>

NUX_API void *nux_os_alloc(void     *userdata,
                           void     *p,
                           nux_u32_t osize,
                           nux_u32_t nsize);
NUX_API void  nux_os_log(void           *userdata,
                         nux_log_level_t level,
                         const nux_c8_t *log,
                         nux_u32_t       len);
NUX_API void  nux_os_stats_update(void *userdata, nux_u64_t *stats);

NUX_API nux_status_t nux_os_file_open(void           *userdata,
                                      nux_u32_t       slot,
                                      const nux_c8_t *path,
                                      nux_u32_t       len,
                                      nux_io_mode_t   mode);
NUX_API void         nux_os_file_close(void *userdata, nux_u32_t slot);
NUX_API nux_status_t nux_os_file_stat(void            *userdata,
                                      nux_u32_t        slot,
                                      nux_file_stat_t *stat);
NUX_API nux_status_t nux_os_file_seek(void     *userdata,
                                      nux_u32_t slot,
                                      nux_u32_t cursor);
NUX_API nux_u32_t    nux_os_file_read(void     *userdata,
                                      nux_u32_t slot,
                                      void     *p,
                                      nux_u32_t n);
NUX_API nux_u32_t    nux_os_file_write(void       *userdata,
                                       nux_u32_t   slot,
                                       const void *p,
                                       nux_u32_t   n);
NUX_API void         nux_os_hotreload_add(void           *userdata,
                                          const nux_c8_t *path,
                                          nux_rid_t       handle);
NUX_API void         nux_os_hotreload_remove(void *userdata, nux_rid_t handle);
NUX_API void         nux_os_hotreload_pull(void      *userdata,
                                           nux_rid_t *handles,
                                           nux_u32_t *count);

NUX_API nux_status_t nux_core_init(void *userdata, const nux_c8_t *entry);
NUX_API void         nux_core_free(void);
NUX_API void         nux_core_update(void);

#endif
