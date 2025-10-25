#include "internal.h"

typedef struct
{

} nux_gui_image_style_t;

static void
draw_image (nux_gui_t g, nu_b2i_t extent, const nux_gui_image_style_t *style)
{
    nu_draw_blit_sliced(ui->active_renderpass,
                        extent,
                        style->extent,
                        style->inner,
                        style->material);
}

nux_gui_t *
nux_gui_new (nux_arena_t *arena, nux_canvas_t *canvas)
{
    nux_gui_t *g = nux_resource_new(arena, NUX_RESOURCE_GUI);
    NUX_CHECK(g, return NUX_NULL);
    g->canvas = nux_resource_rid(canvas);
    NUX_ASSERT(g->canvas);
    g->next_id   = 0;
    g->active_id = 0;
    g->hot_id    = 0;
    return g;
}
nux_b32_t
nux_gui_button (
    nux_gui_t *gui, nux_u32_t x, nux_u32_t y, nux_u32_t w, nux_u32_t h)
{
}
