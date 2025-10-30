#ifndef NUX_IO_INTERNAL_H
#define NUX_IO_INTERNAL_H

#include "module.h"

typedef enum
{
    NUX_DISK_OS,
    NUX_DISK_CART,
} nux_disk_type_t;

typedef enum
{
    NUX_CART_HEADER_SIZE = 4 * 3,
    NUX_CART_ENTRY_SIZE  = 4 * 6,
} nux_cart_layout_t;

struct nux_file_t
{
    nux_disk_type_t type;
    nux_io_mode_t   mode;
    nux_b32_t       is_open;
    union
    {
        struct
        {
            nux_u32_t slot;
            nux_u32_t offset;
            nux_u32_t length;
            nux_u32_t cursor;
        } cart;
        struct
        {
            nux_u32_t slot;
        } os;
    };
};

typedef struct
{
    nux_b32_t compressed;
    nux_u32_t data_type;
    nux_u32_t data_offset;
    nux_u32_t data_length;
    nux_u32_t path_hash;
    nux_u32_t path_offset;
    nux_u32_t path_length;
} nux_cart_entry_t;

typedef struct
{
    const nux_c8_t   *path;
    nux_file_t       *file;
    nux_cart_entry_t *entries;
    nux_u32_t         entries_count;
} nux_cart_t;

typedef struct
{
    nux_b32_t started;
    nux_rid_t file;
    nux_u32_t entry_count;
    nux_u32_t entry_index;
    nux_u32_t cursor;
} nux_cart_writer_t;

typedef struct nux_disk
{
    nux_disk_type_t type;
    union
    {
        nux_cart_t cart;
    };
} nux_disk_t;

typedef struct
{
    nux_cart_writer_t cart_writer;
    nux_u32_vec_t     free_file_slots;
} nux_io_module_t;

const nux_module_info_t *nux_io_module_info(void);

void nux_file_cleanup(void *data);
void nux_disk_cleanup(void *data);

nux_status_t nux_io_open_os_file(const nux_c8_t *path,
                                 nux_io_mode_t   mode,
                                 nux_u32_t      *ret_slot);
void         nux_io_close_os_file(nux_u32_t slot);

#endif
