#include "internal.h"

#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#include <externals/jsmn/jsmn.h>

static void
json_append (nux_serde_json_writer_t *j, const nux_c8_t *s, nux_u32_t n)
{
    nux_ctx_t *ctx = j->ctx;
    nux_io_write(ctx, &j->file, s, n);
}
static nux_status_t
json_writer (const nux_serde_writer_t *s)
{
    nux_serde_json_writer_t *j = s->userdata;
    nux_c8_t                 buf[256];
    nux_u32_t                n;
    if (j->has_previous_value && s->type != NUX_SERDE_END)
    {
        json_append(j, ",", 1);
    }
    if (s->key && j->stack[s->depth] == NUX_SERDE_OBJECT)
    {
        n = nux_snprintf(buf, sizeof(buf), "\"%s\":", s->key);
        json_append(j, buf, n);
    }
    switch (s->type)
    {
        case NUX_SERDE_OBJECT:
            json_append(j, "{", 1);
            j->has_previous_value  = NUX_FALSE;
            j->stack[s->depth + 1] = s->type;
            break;
        case NUX_SERDE_ARRAY:
            json_append(j, "[", 1);
            j->has_previous_value  = NUX_FALSE;
            j->stack[s->depth + 1] = s->type;
            break;
        case NUX_SERDE_END:
            if (j->stack[s->depth + 1] == NUX_SERDE_OBJECT)
            {
                json_append(j, "}", 1);
            }
            else if (j->stack[s->depth + 1] == NUX_SERDE_ARRAY)
            {
                json_append(j, "]", 1);
            }
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

nux_status_t
nux_serde_json_writer_init (nux_serde_json_writer_t *j,
                            nux_ctx_t               *ctx,
                            const nux_c8_t          *path)
{
    j->ctx                = ctx;
    j->has_previous_value = NUX_FALSE;
    nux_serde_writer_init(&j->writer, json_writer, j);
    NUX_CHECK(nux_io_open(ctx, path, NUX_IO_READ_WRITE, &j->file),
              return NUX_FAILURE);
    j->stack[0] = NUX_SERDE_OBJECT;
    nux_serde_write_object(&j->writer, NUX_NULL);
    return NUX_SUCCESS;
}
void
nux_serde_json_writer_close (nux_serde_json_writer_t *j)
{
    nux_serde_write_end(&j->writer);
    nux_io_close(j->ctx, &j->file);
}
nux_status_t
nux_serde_json_reader_init (nux_serde_json_reader_t *j,
                            nux_ctx_t               *ctx,
                            const nux_c8_t          *path)
{
    return NUX_SUCCESS;
}
void
nux_serde_json_reader_close (nux_serde_json_reader_t *j)
{
}
