#include "internal.h"

static void
draw_image (nux_gui_t                    *gui,
            nux_b2i_t                     extent,
            const nux_stylesheet_image_t *style)
{
    nux_canvas_t  *canvas = nux_resource_get(NUX_RESOURCE_CANVAS, gui->canvas);
    nux_texture_t *texture
        = nux_resource_get(NUX_RESOURCE_TEXTURE, style->texture);
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
            return NUX_TRUE;
        }
    }
    return NUX_FALSE;
}
static nux_stylesheet_t *
active_style (nux_gui_t *gui)
{
    nux_stylesheet_t *style = nux_resource_get(
        NUX_RESOURCE_STYLESHEET, *nux_u32_vec_last(&gui->stylesheets));
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
    NUX_CHECK(style, return);
    nux_v2i_t cursor = gui->controllers[0].cursor;
    draw_image(
        gui, nux_b2i(cursor.x - 4, cursor.y - 4, 8, 7), &style->cursor.image);
}

nux_gui_t *
nux_gui_new (nux_arena_t *arena, nux_canvas_t *canvas)
{
    nux_gui_t *gui = nux_resource_new(arena, NUX_RESOURCE_GUI);
    NUX_CHECK(gui, return NUX_NULL);
    gui->canvas = nux_resource_rid(canvas);
    NUX_ASSERT(gui->canvas);
    NUX_CHECK(nux_u32_vec_init_capa(arena, 1, &gui->stylesheets),
              return NUX_NULL);
    gui->next_id   = 0;
    gui->active_id = 0;
    gui->hot_id    = 0;
    for (nux_u32_t i = 0; i < NUX_ARRAY_SIZE(gui->controllers); ++i)
    {
        gui->controllers[i].mode   = NUX_CONTROLLER_MODE_CURSOR;
        gui->controllers[i].active = NUX_FALSE;
        gui->controllers[i].cursor
            = nux_v2i_divs(nux_canvas_get_size(canvas), 2);
        gui->controllers[i].main_pressed = NUX_FALSE;
    }
    return gui;
}
void
nux_gui_push_style (nux_gui_t *gui, nux_stylesheet_t *stylesheet)
{
    NUX_CHECK(stylesheet, return);
    nux_u32_vec_pushv(&gui->stylesheets, nux_resource_rid(stylesheet));
}
void
nux_gui_pop_style (nux_gui_t *gui)
{
    nux_u32_vec_pop(&gui->stylesheets);
}

nux_b32_t
nux_gui_button (
    nux_gui_t *gui, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
    nux_stylesheet_t *style = active_style(gui);
    NUX_CHECK(style, return NUX_FALSE);
    nux_b2i_t extent = nux_b2i(x, y, w, h);

    nux_u32_t id = gui->next_id++;
    nux_u32_t controller;
    nux_b32_t result  = NUX_FALSE;
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
                result = NUX_TRUE;
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
