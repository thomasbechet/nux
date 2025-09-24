#ifndef NUX_IO_INTERNAL_H
#define NUX_IO_INTERNAL_H

#include "module.h"

typedef struct
{
    nux_disk_t        disks[NUX_DISK_MAX];
    nux_u32_t         disks_count;
    nux_u32_vec_t     free_file_slots;
    nux_cart_writer_t cart_writer;
    nux_controller_t  controllers[NUX_CONTROLLER_MAX];
} nux_io_module_t;

nux_io_module_t *nux_io_module(void);

#endif
