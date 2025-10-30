#ifndef NUX_IO_MODULE_H
#define NUX_IO_MODULE_H

#include <io/api.h>
#include <base/module.h>

typedef enum
{
    NUX_SERDE_OBJECT,
    NUX_SERDE_ARRAY,
    NUX_SERDE_END,
    NUX_SERDE_U32,
    NUX_SERDE_F32,
    NUX_SERDE_V3,
    NUX_SERDE_Q4,
    NUX_SERDE_BYTES,
    NUX_SERDE_STRING,
    NUX_SERDE_RID,
    NUX_SERDE_NID,
} nux_serde_type_t;

typedef struct
{
    const nux_c8_t  *key;
    nux_serde_type_t type;
    union
    {
        nux_u32_t *u32;
        nux_f32_t *f32;
        nux_v3_t  *v3;
        nux_q4_t  *q4;
        struct
        {
            const nux_u8_t **data;
            nux_u32_t        n;
        } bytes;
        struct
        {
            const nux_c8_t **s;
            nux_u32_t       *n;
        } str;
        nux_u32_t *size;
    };
} nux_serde_value_t;

struct nux_serde_writer;
struct nux_serde_reader;

typedef nux_status_t (*nux_serde_writer_callback_t)(void *userdata,
                                                    const nux_serde_value_t *v);
typedef nux_status_t (*nux_serde_reader_callback_t)(void              *userdata,
                                                    nux_serde_value_t *v);

typedef struct nux_serde_writer
{
    void                       *userdata;
    nux_serde_writer_callback_t callback;
    nux_status_t                status;
} nux_serde_writer_t;

typedef struct nux_serde_reader
{
    void                       *userdata;
    nux_serde_reader_callback_t callback;
    nux_status_t                status;
} nux_serde_reader_t;

typedef struct
{
    nux_file_t        *file;
    nux_b32_t          has_previous_value;
    nux_serde_writer_t writer;
    nux_serde_type_t   stack[256];
    nux_u32_t          depth;
} nux_json_writer_t;

struct jsmntok;

typedef struct
{
    nux_serde_reader_t reader;
    const nux_c8_t    *json;
    nux_u32_t          json_size;
    void              *tokens;
    nux_u32_t          tokens_capa;
    nux_u32_t          tokens_count;
    struct jsmntok    *it;
    nux_u32_t          depth;
    struct jsmntok    *iters[256];
} nux_json_reader_t;

nux_status_t nux_disk_mount(const nux_c8_t *path);

nux_b32_t    nux_file_exists(const nux_c8_t *path);
nux_file_t  *nux_file_open(nux_arena_t    *arena,
                           const nux_c8_t *path,
                           nux_io_mode_t   mode);
void         nux_file_close(nux_file_t *file);
nux_u32_t    nux_file_read(nux_file_t *file, void *data, nux_u32_t n);
nux_u32_t    nux_file_write(nux_file_t *file, const void *data, nux_u32_t n);
nux_status_t nux_file_seek(nux_file_t *file, nux_u32_t cursor);
nux_status_t nux_file_stat(nux_file_t *file, nux_file_stat_t *stat);
void *nux_file_load(nux_arena_t *arena, const nux_c8_t *path, nux_u32_t *size);

nux_status_t nux_io_write_cart_data(const nux_c8_t *path,
                                    nux_u32_t       type,
                                    nux_b32_t       compress,
                                    const void     *data,
                                    nux_u32_t       size);

void nux_serde_writer_init(nux_serde_writer_t         *s,
                           void                       *userdata,
                           nux_serde_writer_callback_t callback);
void nux_serde_reader_init(nux_serde_reader_t         *s,
                           void                       *userdata,
                           nux_serde_reader_callback_t callback);

nux_status_t nux_json_writer_init(nux_json_writer_t *j, const nux_c8_t *path);
void         nux_json_writer_close(nux_json_writer_t *j);
nux_status_t nux_json_reader_init(nux_json_reader_t *j, const nux_c8_t *path);

void nux_serde_write(nux_serde_writer_t *s, const nux_serde_value_t *value);
void nux_serde_write_object(nux_serde_writer_t *s, const nux_c8_t *key);
void nux_serde_write_array(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           nux_u32_t           size);
void nux_serde_write_end(nux_serde_writer_t *s);
void nux_serde_write_u32(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_u32_t           v);
void nux_serde_write_f32(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_f32_t           v);
void nux_serde_write_v3(nux_serde_writer_t *s, const nux_c8_t *key, nux_v3_t v);
void nux_serde_write_q4(nux_serde_writer_t *s, const nux_c8_t *key, nux_q4_t v);
void nux_serde_write_bytes(nux_serde_writer_t *s,
                           const nux_c8_t     *key,
                           const nux_u8_t     *bytes,
                           nux_u32_t           size);
void nux_serde_write_string(nux_serde_writer_t *s,
                            const nux_c8_t     *key,
                            const nux_c8_t     *v);
void nux_serde_write_rid(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_rid_t           rid);
void nux_serde_write_eid(nux_serde_writer_t *s,
                         const nux_c8_t     *key,
                         nux_nid_t           v);

void nux_serde_read(nux_serde_reader_t *s, nux_serde_value_t *value);
void nux_serde_read_object(nux_serde_reader_t *s, const nux_c8_t *key);
void nux_serde_read_array(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          nux_u32_t          *size);
void nux_serde_read_end(nux_serde_reader_t *s);
void nux_serde_read_u32(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_u32_t          *v);
void nux_serde_read_f32(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_f32_t          *v);
void nux_serde_read_v3(nux_serde_reader_t *s, const nux_c8_t *key, nux_v3_t *v);
void nux_serde_read_q4(nux_serde_reader_t *s, const nux_c8_t *key, nux_q4_t *v);
void nux_serde_read_bytes(nux_serde_reader_t *s,
                          const nux_c8_t     *key,
                          const nux_u8_t    **v,
                          nux_u32_t          *n);
void nux_serde_read_string(nux_serde_reader_t *s,
                           const nux_c8_t     *key,
                           const nux_c8_t    **v,
                           nux_u32_t          *n);
void nux_serde_read_rid(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_rid_t          *rid);
void nux_serde_read_nid(nux_serde_reader_t *s,
                        const nux_c8_t     *key,
                        nux_nid_t          *v);

#endif
