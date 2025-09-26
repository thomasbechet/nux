#ifndef NUX_ECS_INTERNAL_H
#define NUX_ECS_INTERNAL_H

#include "module.h"

#define NUX_EID_INDEX(id)   (id - 1)
#define NUX_EID_MAKE(index) (index + 1)

typedef struct
{
    nux_ecs_t          *active;
    nux_ecs_t          *default_ecs;
    nux_ecs_component_t components[NUX_COMPONENT_MAX];
    nux_u32_t           components_max;
} nux_ecs_module_t;

nux_ecs_module_t *nux_ecs_module(void);

nux_status_t nux_ecs_init(void);
void         nux_ecs_free(void);

#endif
