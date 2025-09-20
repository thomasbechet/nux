#include "internal.h"

#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#include <externals/jsmn/jsmn.h>

#define CHECK(step) \
    step;           \
    if (s->error)   \
    {               \
        goto error; \
    }

static nux_status_t
json_reader (const nux_serde_t *s)
{
    return NUX_SUCCESS;
}
static void
json_append (nux_serde_json_t *j, const nux_c8_t *s, nux_u32_t n)
{
    nux_ctx_t *ctx = j->ctx;
    nux_io_write(ctx, &j->file, s, n);
}
static nux_status_t
json_writer (const nux_serde_t *s)
{
    nux_serde_json_t *j = s->userdata;
    nux_c8_t          buf[256];
    nux_u32_t         n;
    if (j->has_previous_value && s->type != NUX_SERDE_END_ARRAY
        && s->type != NUX_SERDE_END_OBJECT)
    {
        json_append(j, ",", 1);
    }
    if (s->name)
    {
        n = nux_snprintf(buf, sizeof(buf), "\"%s\":", s->name);
        json_append(j, buf, n);
    }
    switch (s->type)
    {
        case NUX_SERDE_BEGIN_OBJECT:
            json_append(j, "{", 1);
            j->has_previous_value = NUX_FALSE;
            break;
        case NUX_SERDE_END_OBJECT:
            json_append(j, "}", 1);
            j->has_previous_value = NUX_FALSE;
            break;
        case NUX_SERDE_BEGIN_ARRAY:
            json_append(j, "[", 1);
            j->has_previous_value = NUX_FALSE;
            break;
        case NUX_SERDE_END_ARRAY:
            json_append(j, "]", 1);
            j->has_previous_value = NUX_FALSE;
            break;
        case NUX_SERDE_U32:
            n = nux_snprintf(buf, sizeof(buf), "%d", *s->value.u32);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
        case NUX_SERDE_V3:
            n = nux_snprintf(buf,
                             sizeof(buf),
                             "[%lf,%lf,%lf]",
                             s->value.v3->x,
                             s->value.v3->y,
                             s->value.v3->z);
            json_append(j, buf, n);
            j->has_previous_value = NUX_TRUE;
            break;
    }
    return NUX_SUCCESS;
}

void
nux_serde_init (nux_serde_t *s, nux_serde_callback_t callback, void *userdata)
{
    s->callback = callback;
    s->userdata = userdata;
    s->name     = NUX_NULL;
    s->error    = NUX_FALSE;
    s->depth    = 0;
}
nux_serde_t *
nux_serde_json_init_write (nux_serde_json_t *j,
                           nux_ctx_t        *ctx,
                           const nux_c8_t   *path)
{
    j->ctx                = ctx;
    j->has_previous_value = NUX_FALSE;
    nux_serde_init(&j->s, json_writer, j);
    NUX_CHECK(nux_io_open(ctx, path, NUX_IO_READ_WRITE, &j->file),
              return NUX_NULL);
    return &j->s;
}
void
nux_serde_json_close (nux_serde_json_t *j)
{
    nux_io_close(j->ctx, &j->file);
}

static void
callback (nux_serde_t *s)
{
    if (!s->callback(s))
    {
        s->error = NUX_TRUE;
    }
}

void
nux_serde_begin_object (nux_serde_t *s, const nux_c8_t *name)
{
    CHECK();
    s->type = NUX_SERDE_BEGIN_OBJECT;
    s->name = NUX_NULL;
    CHECK(callback(s));
    ++s->depth;
error:
    return;
}
void
nux_serde_begin_array (nux_serde_t *s, const nux_c8_t *name, nux_u32_t *size)
{
    CHECK();
    s->type = NUX_SERDE_BEGIN_ARRAY;
    s->name = NUX_NULL;
    CHECK(callback(s));
    ++s->depth;
error:
    return;
}
void
nux_serde_end_object (nux_serde_t *s)
{
    CHECK();
    s->type = NUX_SERDE_END_OBJECT;
    s->name = NUX_NULL;
    NUX_ASSERT(s->depth);
    --s->depth;
    CHECK(callback(s));
error:
    return;
}
void
nux_serde_end_array (nux_serde_t *s)
{
    CHECK();
    s->type = NUX_SERDE_END_ARRAY;
    s->name = NUX_NULL;
    NUX_ASSERT(s->depth);
    --s->depth;
    CHECK(callback(s));
error:
    return;
}
void
nux_serde_u32 (nux_serde_t *s, const nux_c8_t *name, nux_u32_t *v)
{
    CHECK();
    s->type      = NUX_SERDE_U32;
    s->name      = name;
    s->value.u32 = v;
    CHECK(callback(s));
error:
    return;
}
void
nux_serde_v3 (nux_serde_t *s, const nux_c8_t *name, nux_v3_t *v)
{
    CHECK();
    s->type     = NUX_SERDE_V3;
    s->name     = name;
    s->value.v3 = v;
    CHECK(callback(s));
error:
    return;
}
