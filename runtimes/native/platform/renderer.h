#ifndef RENDERER_H
#define RENDERER_H

#include <nulib.h>

nu_status_t renderer_init(void);
void        renderer_free(void);
void        renderer_render(void);

#endif
