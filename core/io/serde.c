#include "internal.h"

void
nux_serde_writer_init (nux_serde_writer_t         *s,
                       void                       *userdata,
                       nux_serde_writer_callback_t callback)
{
    s->userdata = userdata;
    s->callback = callback;
    s->status   = NUX_SUCCESS;
}
void
nux_serde_reader_init (nux_serde_reader_t         *s,
                       void                       *userdata,
                       nux_serde_reader_callback_t callback)
{
    s->userdata = userdata;
    s->callback = callback;
    s->status   = NUX_SUCCESS;
}

void
nux_serde_write (nux_serde_writer_t *s, const nux_serde_value_t *value)
{
    if (s->status)
    {
        s->status = s->callback(s->userdata, value);
    }
}
void
nux_serde_write_object (nux_serde_writer_t *s, const nux_c8_t *key)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_OBJECT;
    nux_serde_write(s, &value);
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
    nux_serde_write(s, &value);
}
void
nux_serde_write_end (nux_serde_writer_t *s)
{
    nux_serde_value_t value;
    value.key  = NUX_NULL;
    value.type = NUX_SERDE_END;
    nux_serde_write(s, &value);
}
void
nux_serde_write_u32 (nux_serde_writer_t *s, const nux_c8_t *key, nux_u32_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_U32;
    value.u32  = &v;
    nux_serde_write(s, &value);
}
void
nux_serde_write_f32 (nux_serde_writer_t *s, const nux_c8_t *key, nux_f32_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_F32;
    value.f32  = &v;
    nux_serde_write(s, &value);
}
void
nux_serde_write_v3 (nux_serde_writer_t *s, const nux_c8_t *key, nux_v3_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_V3;
    value.v3   = &v;
    nux_serde_write(s, &value);
}
void
nux_serde_write_q4 (nux_serde_writer_t *s, const nux_c8_t *key, nux_q4_t v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_Q4;
    value.q4   = &v;
    nux_serde_write(s, &value);
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
    nux_serde_write(s, &value);
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
    nux_serde_write(s, &value);
}
void
nux_serde_write_rid (nux_serde_writer_t *s, const nux_c8_t *key, nux_rid_t rid)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_RID;
    value.u32  = &rid;
    nux_serde_write(s, &value);
}
void
nux_serde_write_eid (nux_serde_writer_t *s, const nux_c8_t *key, nux_eid_t v)
{
    nux_serde_value_t value;
    value.type = NUX_SERDE_EID;
    value.key  = key;
    value.u32  = &v;
    nux_serde_write(s, &value);
}

void
nux_serde_read (nux_serde_reader_t *s, nux_serde_value_t *value)
{
    if (s->status)
    {
        s->status = s->callback(s->userdata, value);
    }
}
void
nux_serde_read_object (nux_serde_reader_t *s, const nux_c8_t *key)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_OBJECT;
    nux_serde_read(s, &value);
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
    nux_serde_read(s, &value);
}
void
nux_serde_read_end (nux_serde_reader_t *s)
{
    nux_serde_value_t value;
    value.key  = NUX_NULL;
    value.type = NUX_SERDE_END;
    nux_serde_read(s, &value);
}
void
nux_serde_read_u32 (nux_serde_reader_t *s, const nux_c8_t *key, nux_u32_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_U32;
    value.u32  = v;
    nux_serde_read(s, &value);
}
void
nux_serde_read_f32 (nux_serde_reader_t *s, const nux_c8_t *key, nux_f32_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_F32;
    value.f32  = v;
    nux_serde_read(s, &value);
}
void
nux_serde_read_v3 (nux_serde_reader_t *s, const nux_c8_t *key, nux_v3_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_V3;
    value.v3   = v;
    nux_serde_read(s, &value);
}
void
nux_serde_read_q4 (nux_serde_reader_t *s, const nux_c8_t *key, nux_q4_t *v)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_Q4;
    value.q4   = v;
    nux_serde_read(s, &value);
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
    nux_serde_read(s, &value);
}
void
nux_serde_read_rid (nux_serde_reader_t *s, const nux_c8_t *key, nux_rid_t *rid)
{
    nux_serde_value_t value;
    value.key  = key;
    value.type = NUX_SERDE_RID;
    value.u32  = rid;
    nux_serde_read(s, &value);
}
void
nux_serde_read_eid (nux_serde_reader_t *s, const nux_c8_t *key, nux_eid_t *v)
{
    nux_serde_value_t value;
    value.type = NUX_SERDE_EID;
    value.key  = key;
    value.u32  = v;
    nux_serde_read(s, &value);
}
