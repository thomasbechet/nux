#ifndef NUX_INPUT_PLATFORM_H
#define NUX_INPUT_PLATFORM_H

#include <input/input.h>

typedef struct
{
    nux_os_event_type_t type;
    union
    {
        nux_input_event_t input;
    };
} nux_os_event_t;

NUX_API void nux_core_push_event(nux_os_event_t *event);

#endif
