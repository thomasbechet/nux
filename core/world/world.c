#include "internal.h"

static nux_world_module_t _module;

static void
world_cleanup (void *data)
{
    nux_world_t *world = data;
    if (_module.active_world == world && world != _module.default_world)
    {
        nux_warning("cleanup active world, default world has been set");
        nux_world_set_active(nullptr);
    }
}

static nux_status_t
module_init (void)
{
    // Register resources
    nux_resource_register(NUX_RESOURCE_WORLD,
                          (nux_resource_info_t) { .name = "world",
                                                  .size = sizeof(nux_world_t),
                                                  .cleanup = world_cleanup });

    // Create default world
    _module.default_world = nux_world_new(nux_arena_core());
    nux_check(_module.default_world, return NUX_FAILURE);
    _module.active_world = _module.default_world;

    return NUX_SUCCESS;
}

nux_world_t *
nux_world_new (nux_arena_t *a)
{
    nux_world_t *world = nux_resource_new(a, NUX_RESOURCE_WORLD);
    nux_check(world, return nullptr);
    world->free = NUX_NULL;
    nux_vec_init_capa(&world->objects, a, NUX_WORLD_DEFAULT_CAPA);
    nux_vec_push(&world->objects); // Reserve first for null
    return world;
}
void
nux_world_set_active (nux_world_t *world)
{
    if (world)
    {
        _module.active_world = world;
    }
    else
    {
        _module.active_world = _module.default_world;
    }
}

void
nux_wcomponent_register (nux_u32_t index, nux_wcomponent_info_t info)
{
    nux_assert(index != 0);
    nux_assert(index < NUX_WORLD_COMPONENT_MAX);
    nux_wcomponent_t *comp = &_module.components[index];
    comp->info             = info;
    _module.components_max = nux_max(_module.components_max, index + 1);
}

nux_oid_t
nux_object_create (void)
{
    nux_world_t  *world  = _module.active_world;
    nux_handlemap_entry_t *object = nullptr;
    if (world->free)
    {
        object      = world->objects.data + world->free;
        world->free = object->free;
    }
    else
    {
        object = nux_vec_push(&world->objects);
        nux_check(object, return NUX_NULL);
    }
    // object->oid
    return NUX_NULL;
}
void
nux_object_delete (nux_oid_t oid)
{
}
