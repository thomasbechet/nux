#include "internal.h"

static nux_status_t
ecs_serializer_callback (void *userdata, const nux_serde_value_t *v)
{
    nux_ecs_serializer_t *s = userdata;
    if (v->type == NUX_SERDE_EID)
    {
    }
    nux_serde_write(&s->serializer.writer, v);
    return NUX_SUCCESS;
}
static nux_status_t
ecs_deserializer_callback (void *userdata, nux_serde_value_t *v)
{
    nux_ecs_deserializer_t *s = userdata;
    nux_serde_read(&s->deserializer.reader, v);
    if (v->type == NUX_SERDE_EID)
    {
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_ecs_serializer_init (nux_ecs_serializer_t *s,
                         nux_ctx_t            *ctx,
                         nux_ecs_t            *ecs,
                         const nux_c8_t       *path)
{
    s->ctx = ctx;
    s->ecs = ecs;
    NUX_CHECK(nux_json_serializer_init(&s->serializer, ctx, path),
              return NUX_FAILURE);
    nux_serde_writer_init(&s->writer, s, ecs_serializer_callback);
    return NUX_SUCCESS;
}
nux_status_t
nux_ecs_deserializer_init (nux_ecs_deserializer_t *s,
                           nux_ctx_t              *ctx,
                           nux_ecs_t              *ecs,
                           const nux_c8_t         *path)
{
    s->ctx = ctx;
    s->ecs = ecs;
    NUX_CHECK(nux_json_deserializer_init(&s->deserializer, ctx, path),
              return NUX_FAILURE);
    nux_serde_reader_init(&s->reader, s, ecs_deserializer_callback);
    return NUX_SUCCESS;
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
nux_serde_read_eid (nux_serde_reader_t *s, const nux_c8_t *key, nux_eid_t *v)
{
    nux_serde_value_t value;
    value.type = NUX_SERDE_EID;
    value.key  = key;
    value.u32  = v;
    nux_serde_read(s, &value);
}

nux_status_t
nux_ecs_write (nux_serde_writer_t *s, const nux_c8_t *key, nux_ecs_t *ecs)
{
    nux_ctx_t *ctx  = ecs->arena->ctx;
    nux_rid_t  iter = nux_ecs_new_iter_any(ctx, ctx->frame_arena_rid);
    nux_ecs_set_active(ctx, ecs->self);
    NUX_CHECK(iter, goto error);
    nux_serde_write_object(s, key);
    nux_serde_write_array(s, "entities", nux_ecs_count(ctx));
    nux_eid_t e = NUX_NULL;
    while ((e = nux_ecs_next(ctx, iter, e)))
    {
        nux_serde_write_object(s, NUX_NULL);
        nux_serde_write_u32(s, "id", e);
        for (nux_u32_t c = 0; c < ctx->ecs->components_max; ++c)
        {
            nux_ecs_component_t *comp = ctx->ecs->components + c;
            if (!comp->name)
            {
                continue;
            }
            const void *data = nux_ecs_get(ctx, e, c);
            if (data)
            {
                if (comp->write)
                {
                    NUX_CHECK(comp->write(s, comp->name, data), goto error);
                }
                else
                {
                    nux_serde_write_object(s, comp->name);
                    nux_serde_write_end(s);
                }
            }
        }
        nux_serde_write_end(s);
    }
    nux_serde_write_end(s);
    nux_serde_write_end(s);
    return NUX_SUCCESS;
error:
    nux_ecs_set_active(ctx, NUX_NULL);
    return NUX_FAILURE;
}
nux_status_t
nux_ecs_read (nux_serde_reader_t *s, const nux_c8_t *key, nux_ecs_t *ecs)
{
    nux_serde_read_object(s, key);
    nux_serde_read_end(s);
    return NUX_SUCCESS;
}
