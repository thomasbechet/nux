#include "internal.h"

static nux_status_t
ecs_writer_callback (void *userdata, const nux_serde_value_t *v)
{
    nux_ecs_writer_t *s = userdata;
    if (v->type == NUX_SERDE_EID)
    {
        nux_eid_t eid       = *v->u32;
        nux_u32_t eid_index = NUX_EID_INDEX(eid);
        nux_u32_t index
            = eid_index < s->entity_count ? s->entity_map[eid_index] : NUX_NULL;
        nux_serde_value_t value = *v;
        value.u32               = &index;
        nux_serde_write(s->output, v);
    }
    else
    {
        nux_serde_write(s->output, v);
    }
    return NUX_SUCCESS;
}
static nux_status_t
ecs_reader_callback (void *userdata, nux_serde_value_t *v)
{
    nux_ecs_reader_t *s = userdata;
    nux_serde_read(s->input, v);
    if (v->type == NUX_SERDE_EID)
    {
        nux_u32_t index = *v->u32;
        *v->u32 = index < s->entity_count ? s->entity_map[index] : NUX_NULL;
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_ecs_writer_init (nux_ecs_writer_t   *s,
                     nux_serde_writer_t *output,
                     nux_ecs_t          *ecs)
{
    s->ecs        = ecs;
    s->output     = output;
    s->entity_map = NUX_NULL;
    nux_serde_writer_init(&s->writer, s, ecs_writer_callback);
    return NUX_SUCCESS;
}
nux_status_t
nux_ecs_reader_init (nux_ecs_reader_t   *s,
                     nux_serde_reader_t *input,
                     nux_ecs_t          *ecs)
{
    s->ecs   = ecs;
    s->input = input;
    nux_serde_reader_init(&s->reader, s, ecs_reader_callback);
    return NUX_SUCCESS;
}

nux_status_t
nux_ecs_write (nux_serde_writer_t *s, const nux_c8_t *key, nux_ecs_t *ecs)
{
    nux_ecs_module_t *module   = nux_ecs_module();
    nux_ecs_t        *prev_ecs = nux_ecs_active();
    nux_ecs_set_active(ecs);

    nux_ecs_iter_t *iter = nux_ecs_new_iter_any(nux_arena_frame());
    NUX_CHECK(iter, return NUX_FAILURE);

    nux_u32_t entity_count = nux_ecs_count();
    // TODO: no entities case ?
    nux_u32_t *entity_map = nux_arena_malloc(
        nux_arena_frame(), sizeof(*entity_map) * entity_count);
    NUX_CHECK(entity_map, return NUX_FAILURE);

    nux_ecs_writer_t writer;
    nux_ecs_writer_init(&writer, s, ecs);
    writer.entity_map   = entity_map;
    writer.entity_count = entity_count;
    s                   = &writer.writer;

    // 1. Fill entity map (global index to local index)
    nux_eid_t it    = NUX_NULL;
    nux_u32_t index = 0;
    while ((it = nux_ecs_next(iter, it)))
    {
        writer.entity_map[NUX_EID_INDEX(it)] = index;
        ++index;
    }

    // 2. Iterate over entities and write components
    nux_serde_write_object(s, key);
    nux_serde_write_array(s, "entities", nux_ecs_count());
    nux_eid_t e            = NUX_NULL;
    nux_u32_t entity_index = 0;
    while ((e = nux_ecs_next(iter, e)))
    {
        // Compute component count
        nux_u32_t component_count = 0;
        for (nux_u32_t c = 0; c < module->components_max; ++c)
        {
            nux_ecs_component_t *comp = module->components + c;
            if (!comp->name)
            {
                continue;
            }
            if (nux_ecs_has(e, c))
            {
                ++component_count;
            }
        }
        // Write components
        nux_serde_write_array(s, NUX_NULL, component_count); // begin entity
        for (nux_u32_t c = 0; c < module->components_max; ++c)
        {
            nux_ecs_component_t *comp = module->components + c;
            if (!comp->name)
            {
                continue;
            }
            const void *data = nux_ecs_get(e, c);
            if (data)
            {
                nux_serde_write_object(s, NUX_NULL);
                nux_serde_write_string(s, "type", comp->name);
                if (comp->write)
                {
                    NUX_CHECK(comp->write(s, data), goto error);
                }
                nux_serde_write_end(s);
            }
        }
        nux_serde_write_end(s); // end entity
    }
    nux_serde_write_end(s); // entities
    nux_serde_write_end(s); // ecs

    nux_ecs_set_active(prev_ecs);
    return NUX_SUCCESS;
error:
    nux_ecs_set_active(prev_ecs);
    return NUX_FAILURE;
}
nux_status_t
nux_ecs_read (nux_serde_reader_t *s, const nux_c8_t *key, nux_ecs_t *ecs)
{
    nux_ecs_module_t *module   = nux_ecs_module();
    nux_ecs_t        *prev_ecs = nux_ecs_active();
    nux_ecs_set_active(ecs);

    nux_serde_read_object(s, key);
    nux_u32_t entity_count;
    nux_serde_read_array(s, "entities", &entity_count);

    nux_u32_t *entity_map = nux_arena_malloc(
        nux_arena_frame(), sizeof(*entity_map) * entity_count);
    NUX_CHECK(entity_map, goto error);

    nux_ecs_reader_t reader;
    nux_ecs_reader_init(&reader, s, ecs);
    reader.entity_map   = entity_map;
    reader.entity_count = entity_count;
    s                   = &reader.reader;

    // 1. Create entities
    for (nux_u32_t i = 0; i < entity_count; ++i)
    {
        entity_map[i] = nux_ecs_create();
        NUX_CHECK(entity_map[i], goto error);
    }

    // 2. Read entities components
    for (nux_u32_t i = 0; i < entity_count; ++i)
    {
        nux_eid_t e = entity_map[i];
        nux_u32_t component_count;
        nux_serde_read_array(s, NUX_NULL, &component_count); // entity
        for (nux_u32_t j = 0; j < component_count; ++j)
        {
            nux_serde_read_object(s, NUX_NULL); // component
            const nux_c8_t *name;
            nux_u32_t       n;
            nux_serde_read_string(s, "type", &name, &n);
            for (nux_u32_t c = 0; c < module->components_max; ++c)
            {
                nux_ecs_component_t *comp = module->components + c;
                if (comp->name && comp->read
                    && nux_strncmp(comp->name, name, n) == 0)
                {
                    void *data = nux_ecs_add(e, c);
                    NUX_CHECK(comp->read(s, data), goto error);
                    break;
                }
            }
            nux_serde_read_end(s); // component
        }
        nux_serde_read_end(s); // entity
    }
    nux_serde_read_end(s); // entities
    nux_serde_read_end(s); // ecs

    nux_ecs_set_active(prev_ecs);
    return NUX_SUCCESS;

error:
    nux_ecs_set_active(prev_ecs);
    return NUX_FAILURE;
}
