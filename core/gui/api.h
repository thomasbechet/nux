#ifndef NUX_GUI_API_H
#define NUX_GUI_API_H

#include <graphics/api.h>

typedef struct nux_gui_t nux_gui_t;

nux_gui_t *nux_gui_new(nux_arena_t *arena, nux_canvas_t *canvas);
nux_b32_t  nux_gui_button(
     nux_gui_t *gui, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h);

#endif
