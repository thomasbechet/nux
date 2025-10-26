#ifndef NUX_GUI_INTERNAL_H
#define NUX_GUI_INTERNAL_H

#include <base/module.h>
#include <gui/api.h>

typedef struct
{
    nux_rid_t texture;
    nux_b2i_t extent;
    nux_b2i_t inner;
} nux_stylesheet_image_t;

struct nux_stylesheet_t
{
    struct
    {
        nux_stylesheet_image_t pressed;
        nux_stylesheet_image_t released;
        nux_stylesheet_image_t hovered;
    } button;
    struct
    {
        nux_stylesheet_image_t checked;
        nux_stylesheet_image_t unchecked;
    } checkbox;
    struct
    {
        nux_stylesheet_image_t image;
    } cursor;
};

struct nux_gui_t
{
    nux_rid_t     canvas;
    nux_rid_vec_t stylesheets;
    struct
    {
        nux_controller_mode_t mode;
        nux_b32_t             active;
        nux_v2i_t             cursor;
        nux_b32_t             main_pressed;
    } controllers[NUX_CONTROLLER_MAX];
    nux_u32_t next_id;
    nux_u32_t active_id;
    nux_u32_t hot_id;
};

nux_status_t nux_gui_init(void);
void         nux_gui_free(void);
void         nux_gui_pre_update(void);
void         nux_gui_post_update(void);

void nux_gui_begin(nux_gui_t *gui);
void nux_gui_end(nux_gui_t *gui);

#endif
