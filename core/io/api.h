#ifndef NUX_IO_API_H
#define NUX_IO_API_H

#include <base/api.h>

typedef enum
{
    NUX_CONTROLLER_MAX = 4,
    NUX_NAME_MAX       = 64,
    NUX_DISK_MAX       = 8,
} nux_io_constants_t;

typedef struct nux_file_t nux_file_t;

nux_status_t nux_io_cart_begin(const nux_c8_t *path, nux_u32_t entry_count);
nux_status_t nux_io_cart_end(void);
nux_status_t nux_io_write_cart_file(const nux_c8_t *path);

#endif
