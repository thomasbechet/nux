#ifndef NUX_SERDE_H
#define NUX_SERDE_H

#include <core/core.h>

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
void nux_serde_write_id(nux_serde_writer_t *s,
                        const nux_c8_t     *key,
                        nux_id_t            id);

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
const nux_c8_t *nux_serde_read_string(nux_serde_reader_t *s,
                                      const nux_c8_t     *key,
                                      nux_u32_t          *n);
void            nux_serde_read_id(nux_serde_reader_t *s,
                                  const nux_c8_t     *key,
                                  nux_id_t           *id);

#endif
