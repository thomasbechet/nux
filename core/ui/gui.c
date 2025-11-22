#include "internal.h"

static void
draw_image (nux_gui_t                    *gui,
            nux_b2i_t                     extent,
            const nux_stylesheet_image_t *style)
{
    nux_canvas_t  *canvas  = nux_object_get(NUX_OBJECT_CANVAS, gui->canvas);
    nux_texture_t *texture = nux_object_get(NUX_OBJECT_TEXTURE, style->texture);
    if (canvas && texture)
    {
        nux_canvas_blit_sliced(
            canvas, texture, extent, style->extent, style->inner);
    }
}
static nux_b32_t
controller_hit (nux_gui_t *gui, nux_b2i_t extent, nux_u32_t *controller)
{
    for (nux_u32_t i = 0; i < NUX_CONTROLLER_MAX; ++i)
    {
        if (nux_b2i_containsi(extent, gui->controllers[i].cursor))
        {
            *controller = i;
            return true;
        }
    }
    return false;
}
static nux_stylesheet_t *
active_style (nux_gui_t *gui)
{
    nux_stylesheet_t *style = nux_object_get(NUX_OBJECT_STYLESHEET,
                                             *nux_vec_last(&gui->stylesheets));
    return style;
}

void
nux_gui_begin (nux_gui_t *gui)
{
    gui->next_id = 1;
    gui->hot_id  = 0;
}
void
nux_gui_end (nux_gui_t *gui)
{
    nux_stylesheet_t *style = active_style(gui);
    nux_check(style, return);
    nux_v2i_t cursor = gui->controllers[0].cursor;
    draw_image(
        gui, nux_b2i(cursor.x - 4, cursor.y - 4, 8, 7), &style->cursor.image);
}

nux_gui_t *
nux_gui_new (nux_arena_t *arena, nux_canvas_t *canvas)
{
    nux_gui_t *gui = nux_object_new(arena, NUX_OBJECT_GUI);
    nux_check(gui, return nullptr);
    gui->canvas = nux_object_id(canvas);
    nux_assert(gui->canvas);
    nux_vec_init_capa(&gui->stylesheets, arena, 1);
    gui->next_id   = 0;
    gui->active_id = 0;
    gui->hot_id    = 0;
    for (nux_u32_t i = 0; i < nux_array_size(gui->controllers); ++i)
    {
        nux_b2i_t viewport         = nux_canvas_viewport(canvas);
        gui->controllers[i].mode   = NUX_CONTROLLER_MODE_CURSOR;
        gui->controllers[i].active = false;
        gui->controllers[i].cursor
            = nux_v2i_divs(nux_v2i(viewport.x, viewport.y), 2);
        gui->controllers[i].main_pressed = false;
    }
    return gui;
}
void
nux_gui_push_style (nux_gui_t *gui, nux_stylesheet_t *stylesheet)
{
    nux_check(stylesheet, return);
    nux_vec_pushv(&gui->stylesheets, nux_object_id(stylesheet));
}
void
nux_gui_pop_style (nux_gui_t *gui)
{
    nux_vec_pop(&gui->stylesheets);
}

nux_b32_t
nux_gui_button (
    nux_gui_t *gui, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    nux_stylesheet_t *style = active_style(gui);
    nux_check(style, return false);
    nux_b2i_t extent = nux_b2i(x, y, w, h);

    nux_u32_t id = gui->next_id++;
    nux_u32_t controller;
    nux_b32_t result  = false;
    nux_b32_t inside  = controller_hit(gui, extent, &controller);
    nux_b32_t pressed = gui->controllers[0].main_pressed;

    if (inside)
    {
        gui->hot_id = id;
    }

    if (gui->active_id == id && !pressed)
    {
        if (inside)
        {
            gui->hot_id = id;
        }
        else
        {
            gui->hot_id = 0;
        }
        gui->active_id = 0;
    }
    else if (gui->hot_id == id)
    {
        if (pressed)
        {
            if (gui->active_id != id)
            {
                result = true;
            }
            gui->active_id = id;
        }
    }

    if (gui->hot_id == id)
    {
        if (gui->active_id == id)
        {
            draw_image(gui, extent, &style->button.pressed);
        }
        else
        {
            draw_image(gui, extent, &style->button.hovered);
        }
    }
    else
    {
        draw_image(gui, extent, &style->button.released);
    }

    return result;
}
