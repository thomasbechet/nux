#include "internal.h"

#define CHECK(step) \
    step;           \
    if (s->error)   \
    {               \
        goto error; \
    }

void
nux_serde_writer_init (nux_serde_writer_t         *s,
                       nux_serde_writer_callback_t callback,
                       void                       *userdata)
{
    s->callback = callback;
    s->userdata = userdata;
    s->key      = NUX_NULL;
    s->error    = NUX_FALSE;
    s->depth    = 0;
}
void
nux_serde_reader_init (nux_serde_reader_t         *s,
                       nux_serde_reader_callback_t callback,
                       void                       *userdata)
{
    s->callback = callback;
    s->userdata = userdata;
    s->key      = NUX_NULL;
    s->error    = NUX_FALSE;
}

static void
writer_callback (nux_serde_writer_t *s)
{
    if (!s->callback(s))
    {
        s->error = NUX_TRUE;
    }
}
void
nux_serde_write_object (nux_serde_writer_t *s, const nux_c8_t *key)
{
    CHECK();
    s->type = NUX_SERDE_OBJECT;
    s->key  = key;
    CHECK(writer_callback(s));
    ++s->depth;
error:
    return;
}
void
nux_serde_write_array (nux_serde_writer_t *s,
                       const nux_c8_t     *key,
                       nux_u32_t           size)
{
    CHECK();
    s->type       = NUX_SERDE_ARRAY;
    s->key        = key;
    s->value.size = size;
    CHECK(writer_callback(s));
    ++s->depth;
error:
    return;
}
void
nux_serde_write_end (nux_serde_writer_t *s)
{
    CHECK();
    s->type = NUX_SERDE_END;
    s->key  = NUX_NULL;
    NUX_ASSERT(s->depth);
    --s->depth;
    CHECK(writer_callback(s));
error:
    return;
}
void
nux_serde_write_u32 (nux_serde_writer_t *s, const nux_c8_t *key, nux_u32_t v)
{
    CHECK();
    s->type      = NUX_SERDE_U32;
    s->key       = key;
    s->value.u32 = &v;
    CHECK(writer_callback(s));
error:
    return;
}
void
nux_serde_write_str (nux_serde_writer_t *s,
                     const nux_c8_t     *key,
                     const nux_c8_t     *v)
{
    CHECK();
    s->type        = NUX_SERDE_STR;
    s->key         = key;
    s->value.str.s = &v;
    CHECK(writer_callback(s));
error:
    return;
}
void
nux_serde_write_v3 (nux_serde_writer_t *s, const nux_c8_t *name, nux_v3_t v)
{
    CHECK();
    s->type     = NUX_SERDE_V3;
    s->key      = name;
    s->value.v3 = &v;
    CHECK(writer_callback(s));
error:
    return;
}

static void
reader_callback (nux_serde_reader_t *s)
{
    if (!s->callback(s))
    {
        s->error = NUX_TRUE;
    }
}
void
nux_serde_read_object (nux_serde_reader_t *s, const nux_c8_t *key)
{
    CHECK();
    s->type = NUX_SERDE_OBJECT;
    s->key  = key;
    CHECK(reader_callback(s));
error:
    return;
}
nux_u32_t
nux_serde_read_array (nux_serde_reader_t *s, const nux_c8_t *key)
{
    CHECK();
    s->type = NUX_SERDE_ARRAY;
    s->key  = key;
    CHECK(reader_callback(s));
    return s->value.size;
error:
    return 0;
}
void
nux_serde_read_end (nux_serde_reader_t *s)
{
    CHECK();
    s->type = NUX_SERDE_END;
    s->key  = NUX_NULL;
    CHECK(reader_callback(s));
error:
    return;
}
nux_u32_t
nux_serde_read_u32 (nux_serde_reader_t *s, const nux_c8_t *key)
{
    CHECK();
    s->type = NUX_SERDE_U32;
    s->key  = key;
    nux_u32_t v;
    s->value.u32 = &v;
    CHECK(reader_callback(s));
    return v;
error:
    return 0;
}
const nux_c8_t *
nux_serde_read_str (nux_serde_reader_t *s, const nux_c8_t *key, nux_u32_t *n)
{
    CHECK();
    s->type = NUX_SERDE_STR;
    s->key  = key;
    const nux_c8_t *str;
    s->value.str.s = &str;
    s->value.str.n = n;
    CHECK(reader_callback(s));
    return str;
error:
    return NUX_NULL;
}
nux_v3_t
nux_serde_read_v3 (nux_serde_reader_t *s, const nux_c8_t *key)
{
    CHECK();
    s->type = NUX_SERDE_V3;
    s->key  = key;
    nux_v3_t v;
    s->value.v3 = &v;
    CHECK(reader_callback(s));
    return v;
error:
    return NUX_V3_ZEROS;
}
const nux_c8_t *
nux_serde_next_key (nux_serde_reader_t *s)
{
    CHECK();
error:
    return NUX_NULL;
}
