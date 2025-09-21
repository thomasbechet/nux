#include "internal.h"

void
serde_write (nux_serde_writer_t *s, nux_serde_value_t *value)
{
    if (s->status)
    {
        s->status = s->callback(s, value);
    }
}
void
serde_read (nux_serde_reader_t *s, nux_serde_value_t *value)
{
    if (s->status)
    {
        s->status = s->callback(s, value);
    }
}

void
nux_serde_writer_init (nux_serde_writer_t         *s,
                       nux_ctx_t                  *ctx,
                       nux_serde_writer_callback_t callback,
                       void                       *userdata)
{
    s->ctx      = ctx;
    s->callback = callback;
    s->userdata = userdata;
    s->status   = NUX_SUCCESS;
    s->depth    = 0;
}
void
nux_serde_reader_init (nux_serde_reader_t         *s,
                       nux_ctx_t                  *ctx,
                       nux_serde_reader_callback_t callback,
                       void                       *userdata)
{
    s->ctx      = ctx;
    s->callback = callback;
    s->userdata = userdata;
    s->status   = NUX_SUCCESS;
}

void
nux_serde_write_object (nux_serde_writer_t *s, const nux_c8_t *key)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_OBJECT;
    serde_write(s, &value);
    ++s->depth;
}
void
nux_serde_write_array (nux_serde_writer_t *s,
                       const nux_c8_t     *key,
                       nux_u32_t           size)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_ARRAY;
    value.size = &size;
    serde_write(s, &value);
    ++s->depth;
}
void
nux_serde_write_end (nux_serde_writer_t *s)
{
    NUX_ASSERT(s->depth);
    --s->depth;
    nux_serde_value_t value;
    value.key  = NUX_NULL;
    value.type = NUX_SERDE_END;
    serde_write(s, &value);
}
void
nux_serde_write_u32 (nux_serde_writer_t *s, const nux_c8_t *key, nux_u32_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_U32;
    value.u32  = &v;
    serde_write(s, &value);
}
void
nux_serde_write_f32 (nux_serde_writer_t *s, const nux_c8_t *key, nux_f32_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_F32;
    value.f32  = &v;
    serde_write(s, &value);
}
void
nux_serde_write_v3 (nux_serde_writer_t *s, const nux_c8_t *key, nux_v3_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_V3;
    value.v3   = &v;
    serde_write(s, &value);
}
void
nux_serde_write_q4 (nux_serde_writer_t *s, const nux_c8_t *key, nux_q4_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_Q4;
    value.q4   = &v;
    serde_write(s, &value);
}
void
nux_serde_write_bytes (nux_serde_writer_t *s,
                       const nux_c8_t     *key,
                       const nux_u8_t     *bytes,
                       nux_u32_t           size)
{
    nux_serde_value_t value;
    value.key        = key;
    value.type       = NUX_SERDE_BYTES;
    value.bytes.data = &bytes;
    value.bytes.n    = size;
    serde_write(s, &value);
}
void
nux_serde_write_string (nux_serde_writer_t *s,
                        const nux_c8_t     *key,
                        const nux_c8_t     *v)
{
    nux_serde_value_t value;
    value.key   = key;
    value.type  = NUX_SERDE_STRING;
    value.str.s = &v;
    serde_write(s, &value);
}
void
nux_serde_write_rid (nux_serde_writer_t *s, const nux_c8_t *key, nux_rid_t rid)
{
    const nux_c8_t *name = nux_resource_get_name(s->ctx, rid);
    NUX_CHECK(name, return);
}

void
nux_serde_read_object (nux_serde_reader_t *s, const nux_c8_t *key)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_OBJECT;
    serde_read(s, &value);
}
void
nux_serde_read_array (nux_serde_reader_t *s,
                      const nux_c8_t     *key,
                      nux_u32_t          *size)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_ARRAY;
    value.size = size;
    serde_read(s, &value);
}
void
nux_serde_read_end (nux_serde_reader_t *s)
{
    nux_serde_value_t value;
    value.key  = NUX_NULL;
    value.type = NUX_SERDE_END;
    serde_read(s, &value);
}
void
nux_serde_read_u32 (nux_serde_reader_t *s, const nux_c8_t *key, nux_u32_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_U32;
    value.u32  = v;
    serde_read(s, &value);
}
void
nux_serde_read_f32 (nux_serde_reader_t *s, const nux_c8_t *key, nux_f32_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_F32;
    value.f32  = v;
    serde_read(s, &value);
}
void
nux_serde_read_v3 (nux_serde_reader_t *s, const nux_c8_t *key, nux_v3_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_V3;
    value.v3   = v;
    serde_read(s, &value);
}
void
nux_serde_read_q4 (nux_serde_reader_t *s, const nux_c8_t *key, nux_q4_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_Q4;
    value.q4   = v;
    serde_read(s, &value);
}
void
nux_serde_read_string (nux_serde_reader_t *s,
                       const nux_c8_t     *key,
                       const nux_c8_t    **v,
                       nux_u32_t          *n)
{
    nux_serde_value_t value;
    value.key   = key;
    value.type  = NUX_SERDE_STRING;
    value.str.s = v;
    value.str.n = n;
    serde_read(s, &value);
}
void
nux_serde_read_rid (nux_serde_reader_t *s, const nux_c8_t *key, nux_rid_t *rid)
{
}
