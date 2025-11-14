#ifndef NUX_WORLD_INTERNAL_H
#define NUX_WORLD_INTERNAL_H

#include <world/world.h>

#define NUX_OID_INDEX(id)   (id - 1)
#define NUX_OID_MAKE(index) (index + 1)

typedef enum
{
    NUX_WORLD_COMPONENT_MAX = 26,
    NUX_WORLD_DEFAULT_CAPA  = 1024,
} nux_world_constants_t;

typedef struct
{
    nux_wcomponent_info_t info;
} nux_wcomponent_t;

typedef struct
{
    nux_oid_t oid;
    union
    {
        struct
        {
            nux_u32_t parent;
            nux_u32_t next;
            nux_u32_t prev;
            nux_u32_t child;
            nux_u32_t flags;
            nux_u32_t components[NUX_WORLD_COMPONENT_MAX];
        };
        nux_u32_t free;
    };
} nux_object_t;

typedef struct nux_world_t
{
    nux_vec(nux_object_t) objects;
    nux_u32_t free;
} nux_world_t;

typedef struct
{
    nux_world_t     *active_world;
    nux_world_t     *default_world;
    nux_wcomponent_t components[NUX_WORLD_COMPONENT_MAX];
    nux_u32_t        components_max;
} nux_world_module_t;

#endif
