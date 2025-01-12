#ifndef VMN_RENDERER_H
#define VMN_RENDERER_H

#include <vmcore/vm.h>

nu_status_t vmn_renderer_init(const vm_config_t *config);
void        vmn_renderer_free(void);
void        vmn_renderer_render(void);

#endif
