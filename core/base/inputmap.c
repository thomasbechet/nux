#include "internal.h"

static nux_inputmap_entry_t *
find_entry (nux_inputmap_t *map, const nux_c8_t *name)
{
    for (nux_u32_t i = 0; i < map->entries.size; ++i)
    {
        nux_inputmap_entry_t *entry = map->entries.data + i;
        if (nux_strncmp(entry->name, name, NUX_NAME_MAX) == 0)
        {
            return entry;
        }
    }

    nux_arena_t          *a     = nux_resource_arena(nux_resource_rid(map));
    nux_inputmap_entry_t *entry = nux_inputmap_entry_vec_push(&map->entries);
    NUX_CHECK(entry, return NUX_NULL);
    entry->name = nux_arena_alloc_string(a, name);
    entry->type = NUX_INPUT_UNMAPPED;

    // Update controllers
    nux_base_module_t *module = nux_base_module();
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(module->controllers); ++i)
    {
        nux_controller_t *controller = module->controllers + i;
        if (controller->inputmap == nux_resource_rid(map))
        {
            NUX_CHECK(
                nux_f32_vec_resize(&controller->inputs, map->entries.size),
                return NUX_NULL);
        }
    }

    return entry;
}

nux_status_t
nux_inputmap_find_index (const nux_inputmap_t *map,
                         const nux_c8_t       *name,
                         nux_u32_t            *index)
{
    for (nux_u32_t i = 0; i < map->entries.size; ++i)
    {
        nux_inputmap_entry_t *entry = map->entries.data + i;
        if (nux_strncmp(entry->name, name, NUX_NAME_MAX) == 0)
        {
            *index = i;
            return NUX_SUCCESS;
        }
    }
    return NUX_FAILURE;
}

nux_inputmap_t *
nux_inputmap_new (nux_arena_t *arena)
{
    nux_inputmap_t *map = nux_resource_new(arena, NUX_RESOURCE_INPUTMAP);
    NUX_CHECK(map, return NUX_NULL);
    NUX_CHECK(nux_inputmap_entry_vec_init_capa(
                  arena, DEFAULT_CONTROLLER_INPUT_SIZE, &map->entries),
              return NUX_NULL);
    return map;
}
void
nux_inputmap_bind_key (nux_inputmap_t *map, const nux_c8_t *name, nux_key_t key)
{
    nux_inputmap_entry_t *entry = find_entry(map, name);
    NUX_CHECK(entry, return);
    entry->type = NUX_INPUT_KEY;
    entry->key  = key;
}
void
nux_inputmap_bind_mouse_button (nux_inputmap_t    *map,
                                const nux_c8_t    *name,
                                nux_mouse_button_t button)
{
    nux_inputmap_entry_t *entry = find_entry(map, name);
    NUX_CHECK(entry, return);
    entry->type         = NUX_INPUT_MOUSE_BUTTON;
    entry->mouse_button = button;
}
void
nux_inputmap_bind_mouse_axis (nux_inputmap_t  *map,
                              const nux_c8_t  *name,
                              nux_mouse_axis_t axis,
                              nux_f32_t        sensivity)
{
    nux_inputmap_entry_t *entry = find_entry(map, name);
    NUX_CHECK(entry, return);
    entry->type       = NUX_INPUT_MOUSE_AXIS;
    entry->mouse_axis = axis;
    entry->sensivity  = sensivity;
}
