#ifndef NUX_IO_INTERNAL_H
#define NUX_IO_INTERNAL_H

#include <base/internal.h>

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef enum
{
    NUX_CART_HEADER_SIZE = 4 * 3,
    NUX_CART_ENTRY_SIZE  = 4 * 6,
} nux_cart_layout_t;

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
    nux_b32_t started;
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

typedef enum
{
    NUX_CONTROLLER_MODE_MOTION,
    NUX_CONTROLLER_MODE_CURSOR,
} nux_controller_mode_t;

typedef struct
{
    nux_controller_mode_t mode;

    nux_u32_t buttons;
    nux_u32_t buttons_prev;
    nux_f32_t axis[NUX_AXIS_MAX];
    nux_f32_t axis_prev[NUX_AXIS_MAX];

    nux_v2_t cursor;
    nux_v2_t cursor_prev;

    nux_button_t cursor_motion_buttons[4];
    nux_axis_t   cursor_motion_axis[2];
    nux_f32_t    cursor_motion_speed;
} nux_controller_t;

typedef enum
{
    NUX_SERDE_OBJECT,
    NUX_SERDE_ARRAY,
    NUX_SERDE_U32,
} nux_serde_type_t;

typedef union
{
    nux_u32_t *u32;
    nux_b32_t  begin;
} nux_serde_value_t;

struct nux_serde;
typedef nux_status_t (*nux_serde_callback_t)(const struct nux_serde *s);

typedef struct nux_serde
{
    nux_serde_callback_t callback;
    void                *userdata;
    const nux_c8_t      *name;
    nux_serde_type_t     type;
    nux_serde_value_t    value;
    nux_u32_t            depth;
    nux_serde_type_t     stack[16];
    nux_b32_t            error;
} nux_serde_t;

typedef struct
{
    nux_ctx_t  *ctx;
    nux_file_t  file;
    nux_serde_t s;
} nux_serde_json_t;

typedef struct nux_io_module
{
    nux_disk_t        disks[NUX_DISK_MAX];
    nux_u32_t         disks_count;
    nux_u32_vec_t     free_file_slots;
    nux_cart_writer_t cart_writer;
    nux_controller_t  controllers[NUX_CONTROLLER_MAX];
} nux_io_module_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// io.c

nux_status_t nux_io_init(nux_ctx_t *ctx);
void         nux_io_free(nux_ctx_t *ctx);

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
void        *nux_io_load(nux_ctx_t      *ctx,
                         nux_rid_t       arena,
                         const nux_c8_t *path,
                         nux_u32_t      *size);

nux_status_t nux_io_write_cart_data(nux_ctx_t      *ctx,
                                    const nux_c8_t *path,
                                    nux_u32_t       type,
                                    nux_b32_t       compress,
                                    const void     *data,
                                    nux_u32_t       size);

// file.c

void nux_file_cleanup(nux_ctx_t *ctx, nux_rid_t res);

// input.c

void nux_input_update(nux_ctx_t *ctx);

// serde.c

void         nux_serde_init(nux_serde_t         *s,
                            nux_serde_callback_t callback,
                            void                *userdata);
void         nux_serde_init_dump(nux_serde_t *s, nux_ctx_t *ctx);
nux_serde_t *nux_serde_json_init_write(nux_serde_json_t *j,
                                       nux_ctx_t        *ctx,
                                       const nux_c8_t   *path);
void         nux_serde_json_close(nux_serde_json_t *j);

void nux_serde_begin_object(nux_serde_t *s, const nux_c8_t *name);
void nux_serde_begin_array(nux_serde_t    *s,
                           const nux_c8_t *name,
                           nux_u32_t      *size);
void nux_serde_end(nux_serde_t *s);
void nux_serde_u32(nux_serde_t *s, const nux_c8_t *name, nux_u32_t *v);
const nux_c8_t *nux_serde_next(nux_serde_t *s);

#endif
