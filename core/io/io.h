#ifndef NUX_IO_H
#define NUX_IO_H

#include "base/base.h"

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef enum
{
    NUX_DISK_OS,
    NUX_DISK_CART,
} nux_disk_type_t;

typedef struct
{
    nux_disk_type_t type;
    nux_io_mode_t   mode;
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
} nux_file_t;

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
    nux_c8_t          path[NUX_PATH_BUF_SIZE];
    nux_u32_t         slot;
    nux_cart_entry_t *entries;
    nux_u32_t         entries_count;
} nux_cart_t;

typedef struct
{
    nux_u32_t slot;
    nux_u32_t entry_count;
    nux_u32_t entry_index;
    nux_u32_t cursor;
} nux_cart_writer_t;

typedef struct
{
    nux_disk_type_t type;
    union
    {
        nux_cart_t cart;
    };
} nux_disk_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// io.c

nux_status_t nux_io_init(nux_ctx_t *ctx);
nux_status_t nux_io_free(nux_ctx_t *ctx);

nux_status_t nux_io_mount(nux_ctx_t *ctx, const nux_c8_t *path);

nux_b32_t    nux_io_exists(nux_ctx_t *ctx, const nux_c8_t *path);
nux_status_t nux_io_open(nux_ctx_t      *ctx,
                         const nux_c8_t *path,
                         nux_io_mode_t   mode,
                         nux_file_t     *file);
void         nux_io_close(nux_ctx_t *ctx, nux_file_t *file);
nux_u32_t    nux_io_read(nux_ctx_t  *ctx,
                         nux_file_t *file,
                         void       *data,
                         nux_u32_t   n);
nux_u32_t    nux_io_write(nux_ctx_t  *ctx,
                          nux_file_t *file,
                          const void *data,
                          nux_u32_t   n);
nux_status_t nux_io_seek(nux_ctx_t *ctx, nux_file_t *file, nux_u32_t cursor);
nux_status_t nux_io_stat(nux_ctx_t       *ctx,
                         nux_file_t      *file,
                         nux_file_stat_t *stat);
void        *nux_io_load(nux_ctx_t *ctx, const nux_c8_t *path, nux_u32_t *size);

nux_status_t nux_io_write_cart_data(nux_ctx_t      *ctx,
                                    const nux_c8_t *path,
                                    nux_u32_t       type,
                                    nux_b32_t       compress,
                                    const void     *data,
                                    nux_u32_t       size);

// file.c

void nux_file_cleanup(nux_ctx_t *ctx, void *data);

// input.c

void nux_input_pre_update(nux_ctx_t *ctx);

#endif
