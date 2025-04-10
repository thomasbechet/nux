#include "runtime.h"

static const nu_char_t *button_to_name[NUX_BUTTON_MAX]
    = { "A", "X", "Y", "B", "UP", "DOWN", "LEFT", "RIGHT", "LB", "RB" };
static const nu_char_t *axis_to_name[NUX_AXIS_MAX]
    = { "LEFTX", "LEFTY", "RIGHTX", "RIGHTY", "RT", "LT" };

void
view_controls (struct nk_context *ctx, struct nk_rect bounds)
{
    const nu_size_t row = 30;
    if (nk_begin(ctx, "Controls", bounds, NK_WINDOW_BACKGROUND))
    {
        nk_layout_row_template_begin(ctx, row);
        nk_layout_row_template_push_static(ctx, 60);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_end(ctx);

        for (nu_size_t i = 0; i < NUX_BUTTON_MAX; ++i)
        {
            nk_label(ctx, button_to_name[i], NK_TEXT_LEFT);
            if (nk_button_label(ctx, "A"))
            {
            }
        }
        for (nu_size_t i = 0; i < NUX_AXIS_MAX; ++i)
        {
            nk_label(ctx, axis_to_name[i], NK_TEXT_LEFT);
            if (nk_button_label(ctx, "A"))
            {
            }
        }
    }
    nk_end(ctx);
}
