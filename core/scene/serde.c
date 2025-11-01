#include "internal.h"

static nux_status_t
scene_writer_callback (void *userdata, const nux_serde_value_t *v)
{
    nux_scene_writer_t *s = userdata;
    if (v->type == NUX_SERDE_NID)
    {
        nux_nid_t nid       = *v->u32;
        nux_u32_t nid_index = NUX_NID_INDEX(nid);
        nux_u32_t index
            = nid_index < s->node_count ? s->node_map[nid_index] : NUX_NULL;
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
scene_reader_callback (void *userdata, nux_serde_value_t *v)
{
    nux_scene_reader_t *s = userdata;
    nux_serde_read(s->input, v);
    if (v->type == NUX_SERDE_NID)
    {
        nux_u32_t index = *v->u32;
        *v->u32         = index < s->node_count ? s->node_map[index] : NUX_NULL;
    }
    return NUX_SUCCESS;
}

nux_status_t
nux_scene_writer_init (nux_scene_writer_t *s,
                       nux_serde_writer_t *output,
                       nux_scene_t        *scene)
{
    s->scene    = scene;
    s->output   = output;
    s->node_map = NUX_NULL;
    nux_serde_writer_init(&s->writer, s, scene_writer_callback);
    return NUX_SUCCESS;
}
nux_status_t
nux_scene_reader_init (nux_scene_reader_t *s,
                       nux_serde_reader_t *input,
                       nux_scene_t        *scene)
{
    s->scene = scene;
    s->input = input;
    nux_serde_reader_init(&s->reader, s, scene_reader_callback);
    return NUX_SUCCESS;
}

nux_status_t
nux_scene_write (nux_serde_writer_t *s, const nux_c8_t *key, nux_scene_t *scene)
{
    nux_scene_module_t *module     = nux_scene_module();
    nux_scene_t        *prev_scene = nux_scene_active();
    nux_scene_set_active(scene);

    nux_query_t *iter = nux_query_new_any(nux_arena_frame());
    NUX_CHECK(iter, return NUX_FAILURE);

    nux_u32_t entity_count = nux_scene_count();
    // TODO: no entities case ?
    nux_u32_t *entity_map
        = nux_malloc(nux_allocator_frame(), sizeof(*entity_map) * entity_count);
    NUX_CHECK(entity_map, return NUX_FAILURE);

    nux_scene_writer_t writer;
    nux_scene_writer_init(&writer, s, scene);
    writer.node_map   = entity_map;
    writer.node_count = entity_count;
    s                 = &writer.writer;

    // 1. Fill entity map (global index to local index)
    nux_nid_t it    = NUX_NULL;
    nux_u32_t index = 0;
    while ((it = nux_query_next(iter, it)))
    {
        writer.node_map[NUX_NID_INDEX(it)] = index;
        ++index;
    }

    // 2. Iterate over entities and write components
    nux_serde_write_object(s, key);
    nux_serde_write_array(s, "entities", nux_scene_count());
    nux_nid_t e            = NUX_NULL;
    nux_u32_t entity_index = 0;
    while ((e = nux_query_next(iter, e)))
    {
        // Compute component count
        nux_u32_t component_count = 0;
        for (nux_u32_t c = 0; c < module->components_max; ++c)
        {
            nux_component_t *comp = module->components + c;
            if (!comp->name)
            {
                continue;
            }
            if (nux_node_has(e, c))
            {
                ++component_count;
            }
        }
        // Write components
        nux_serde_write_array(s, NUX_NULL, component_count); // begin entity
        for (nux_u32_t c = 0; c < module->components_max; ++c)
        {
            nux_component_t *comp = module->components + c;
            if (!comp->name)
            {
                continue;
            }
            const void *data = nux_component_get(e, c);
            if (data)
            {
                nux_serde_write_object(s, NUX_NULL);
                nux_serde_write_string(s, "component", comp->name);
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
    nux_serde_write_end(s); // scene

    nux_scene_set_active(prev_scene);
    return NUX_SUCCESS;
error:
    nux_scene_set_active(prev_scene);
    return NUX_FAILURE;
}
nux_status_t
nux_scene_read (nux_serde_reader_t *s, const nux_c8_t *key, nux_scene_t *scene)
{
    nux_scene_module_t *module     = nux_scene_module();
    nux_scene_t        *prev_scene = nux_scene_active();
    nux_scene_set_active(scene);

    nux_serde_read_object(s, key);
    nux_u32_t entity_count;
    nux_serde_read_array(s, "entities", &entity_count);

    nux_u32_t *entity_map
        = nux_malloc(nux_allocator_frame(), sizeof(*entity_map) * entity_count);
    NUX_CHECK(entity_map, goto error);

    nux_scene_reader_t reader;
    nux_scene_reader_init(&reader, s, scene);
    reader.node_map   = entity_map;
    reader.node_count = entity_count;
    s                 = &reader.reader;

    // 1. Create entities
    for (nux_u32_t i = 0; i < entity_count; ++i)
    {
        entity_map[i] = nux_node_create(nux_node_root());
        NUX_CHECK(entity_map[i], goto error);
    }

    // 2. Read entities components
    for (nux_u32_t i = 0; i < entity_count; ++i)
    {
        nux_nid_t e = entity_map[i];
        nux_u32_t component_count;
        nux_serde_read_array(s, NUX_NULL, &component_count); // entity
        for (nux_u32_t j = 0; j < component_count; ++j)
        {
            nux_serde_read_object(s, NUX_NULL); // component
            const nux_c8_t *name;
            nux_u32_t       n;
            nux_serde_read_string(s, "component", &name, &n);
            for (nux_u32_t c = 0; c < module->components_max; ++c)
            {
                nux_component_t *comp = module->components + c;
                if (comp->name && comp->read
                    && nux_strncmp(comp->name, name, n) == 0)
                {
                    nux_node_add(e, c);
                    void *data = nux_component_get(e, c);
                    NUX_CHECK(comp->read(s, data), goto error);
                    break;
                }
            }
            nux_serde_read_end(s); // component
        }
        nux_serde_read_end(s); // entity
    }
    nux_serde_read_end(s); // entities
    nux_serde_read_end(s); // scene

    nux_scene_set_active(prev_scene);
    return NUX_SUCCESS;

error:
    nux_scene_set_active(prev_scene);
    return NUX_FAILURE;
}
