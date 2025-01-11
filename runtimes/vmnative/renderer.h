#ifndef NUX_RENDERER_H
#define NUX_RENDERER_H

#include "error.h"

#include <vmcore/vm.h>

nux_error_code_t nux_renderer_init(const nux_vm_config_t *config);
nux_error_code_t nux_renderer_free(void);
void             nux_renderer_render(void);

#endif
