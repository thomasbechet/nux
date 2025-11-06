#ifndef NUX_GUI_API_H
#define NUX_GUI_API_H

#include <graphics/api.h>
#include <input/api.h>

typedef struct nux_gui_t        nux_gui_t;
typedef struct nux_stylesheet_t nux_stylesheet_t;

typedef enum
{
    NUX_CONTROLLER_MODE_SELECTION = 0,
    NUX_CONTROLLER_MODE_CURSOR    = 1,
} nux_controller_mode_t;

typedef enum
{
    NUX_STYLESHEET_BUTTON_PRESSED     = 0,
    NUX_STYLESHEET_BUTTON_RELEASED    = 1,
    NUX_STYLESHEET_BUTTON_HOVERED     = 2,
    NUX_STYLESHEET_CHECKBOX_CHECKED   = 3,
    NUX_STYLESHEET_CHECKBOX_UNCHECKED = 4,
    NUX_STYLESHEET_CURSOR             = 5
} nux_stylesheet_property_t;

nux_stylesheet_t *nux_stylesheet_new(nux_arena_t *arena);
void              nux_stylesheet_set(nux_stylesheet_t         *style,
                                     nux_stylesheet_property_t property,
                                     nux_texture_t            *texture,
                                     nux_b2i_t                 extent,
                                     nux_b2i_t                 inner);

nux_gui_t *nux_gui_new(nux_arena_t *arena, nux_canvas_t *canvas);
void       nux_gui_push_style(nux_gui_t *gui, nux_stylesheet_t *stylesheet);
void       nux_gui_pop_style(nux_gui_t *gui);

nux_b32_t nux_gui_button(
    nux_gui_t *gui, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h);

#endif
