#ifndef NUX_CORE_H
#define NUX_CORE_H

#include <base/platform.h>
#include <io/platform.h>
#include <graphics/platform.h>
#include <input/api.h>

typedef struct
{
    nux_os_event_type_t type;
    union
    {
        nux_input_event_t input;
    };
} nux_os_event_t;

NUX_API nux_status_t nux_core_init(void *userdata, const nux_c8_t *entry);
NUX_API void         nux_core_free(void);
NUX_API void         nux_core_update(void);
NUX_API void         nux_core_push_event(nux_os_event_t *event);

#endif
