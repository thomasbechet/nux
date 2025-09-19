#include "internal.h"

#include <externals/cgltf/cgltf.h>

#define CHECK(step) \
    step;           \
    if (s->error)   \
    {               \
        goto error; \
    }

static nux_status_t
dumper (const nux_serde_t *s)
{
    nux_ctx_t *ctx = s->userdata;
    switch (s->type)
    {
        case NUX_SERDE_OBJECT:
            if (s->value.begin)
            {
                NUX_INFO("%s : {", s->name);
            }
            else
            {
                NUX_INFO("}");
            }
            break;
        case NUX_SERDE_ARRAY:
            if (s->value.begin)
            {
                NUX_INFO("%s : [", s->name);
            }
            else
            {
                NUX_INFO("]");
            }
            break;
        case NUX_SERDE_U32:
            NUX_INFO("%s : %d", s->name, *s->value.u32);
            break;
    }
    return NUX_SUCCESS;
}

static nux_status_t
json_dumper (const nux_serde_t *s)
{
    nux_serde_json_t *j   = s->userdata;
    nux_ctx_t        *ctx = j->ctx;
    nux_c8_t          buf[256];
    nux_u32_t         n;
    if (s->name)
    {
        n = nux_snprintf(buf, sizeof(buf), "\"%s\":", s->name);
        nux_io_write(ctx, &j->file, buf, n);
    }
    switch (s->type)
    {
        case NUX_SERDE_OBJECT:
            nux_io_write(ctx, &j->file, s->value.begin ? "{" : "}", 1);
            break;
        case NUX_SERDE_ARRAY:
            nux_io_write(ctx, &j->file, s->value.begin ? "[" : "]", 1);
            break;
        case NUX_SERDE_U32:
            n = nux_snprintf(buf, sizeof(buf), "%d,", *s->value.u32);
            nux_io_write(ctx, &j->file, buf, n);
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
void
nux_serde_init_dump (nux_serde_t *s, nux_ctx_t *ctx)
{
    nux_serde_init(s, dumper, ctx);
}
nux_serde_t *
nux_serde_json_init_write (nux_serde_json_t *j,
                           nux_ctx_t        *ctx,
                           const nux_c8_t   *path)
{
    j->ctx = ctx;
    nux_serde_init(&j->s, json_dumper, j);
    NUX_CHECK(nux_io_open(ctx, "test.json", NUX_IO_READ_WRITE, &j->file),
              return NUX_NULL);
    return &j->s;
}
void
nux_serde_json_close (nux_serde_json_t *j)
{
    nux_io_close(j->ctx, &j->file);
}

static nux_b32_t
push_stack (nux_serde_t *s, nux_serde_type_t type)
{
    if (s->depth >= NUX_ARRAY_SIZE(s->stack))
    {
        s->error = NUX_TRUE;
        return NUX_FALSE;
    }
    s->stack[s->depth] = type;
    s->type            = type;
    s->value.begin     = NUX_TRUE;
    ++s->depth;
    return NUX_TRUE;
}
static nux_b32_t
pop_stack (nux_serde_t *s)
{
    if (s->depth == 0)
    {
        s->error = NUX_TRUE;
        return NUX_FALSE;
    }
    --s->depth;
    s->type        = s->stack[s->depth];
    s->value.begin = NUX_FALSE;
    return NUX_TRUE;
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
    CHECK(push_stack(s, NUX_SERDE_OBJECT));
    s->name = NUX_NULL;
    CHECK(callback(s));
error:
    return;
}
void
nux_serde_begin_array (nux_serde_t *s, const nux_c8_t *name, nux_u32_t *size)
{
    CHECK();
    CHECK(push_stack(s, NUX_SERDE_ARRAY));
    s->name = NUX_NULL;
    CHECK(callback(s));
error:
    return;
}
void
nux_serde_end (nux_serde_t *s)
{
    CHECK();
    CHECK(pop_stack(s));
    s->name = NUX_NULL;
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
