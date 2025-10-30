#ifndef NUX_INPUT_MODULE_H
#define NUX_INPUT_MODULE_H

#include <input/api.h>

nux_status_t nux_controller_resize_values(nux_inputmap_t *map);

// inputmap.c

nux_status_t nux_inputmap_find_index(const nux_inputmap_t *map,
                                     const nux_c8_t       *name,
                                     nux_u32_t            *index);

#endif
