#include "internal.h"

static nux_world_module_t _module;

static void
world_cleanup (void *data)
{
    nux_world_t *world = data;
    if (_module.active_world == world && world != _module.default_world)
    {
        NUX_WARNING("cleanup active world, default world has been set");
        nux_world_set_active(NUX_NULL);
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
    NUX_CHECK(_module.default_world, return NUX_FAILURE);
    _module.active_world = _module.default_world;

    return NUX_SUCCESS;
}

nux_world_t *
nux_world_new (nux_arena_t *a)
{
    nux_world_t *world = nux_resource_new(a, NUX_RESOURCE_WORLD);
    NUX_CHECK(world, return NUX_NULL);
    world->free = NUX_NULL;
    NUX_CHECK(
        nux_object_vec_init_capa(a, NUX_WORLD_DEFAULT_CAPA, &world->objects),
        return NUX_NULL);
    nux_object_vec_push(&world->objects); // Reserve first for null
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
    NUX_ASSERT(index != 0);
    NUX_ASSERT(index < NUX_WORLD_COMPONENT_MAX);
    nux_wcomponent_t *comp = &_module.components[index];
    comp->info             = info;
    _module.components_max = NUX_MAX(_module.components_max, index + 1);
}

nux_oid_t
nux_object_create (void)
{
    nux_world_t  *world  = _module.active_world;
    nux_object_t *object = NUX_NULL;
    if (world->free)
    {
        object      = world->objects.data + world->free;
        world->free = object->free;
    }
    else
    {
        object = nux_object_vec_push(&world->objects);
        NUX_CHECK(object, return NUX_NULL);
    }
    // object->oid
}
void
nux_object_delete (nux_oid_t oid)
{
}
