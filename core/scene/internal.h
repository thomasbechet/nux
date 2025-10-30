#ifndef NUX_SCENE_INTERNAL_H
#define NUX_SCENE_INTERNAL_H

#include "module.h"

#define NUX_NID_INDEX(id)   (id - 1)
#define NUX_NID_MAKE(index) (index + 1)

typedef struct
{
    nux_scene_t    *active;
    nux_scene_t    *default_scene;
    nux_component_t components[NUX_COMPONENT_MAX];
    nux_u32_t       components_max;
    nux_query_t    *iter_any;
} nux_scene_module_t;

const nux_module_t *nux_scene_module_info(void);
nux_scene_module_t *nux_scene_module(void);

#endif
