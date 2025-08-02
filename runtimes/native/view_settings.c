#include "internal.h"

void
view_settings (struct nk_context *ctx, struct nk_rect bounds)
{
    const int row = 30;
    if (nk_begin(ctx, "Settings", bounds, NK_WINDOW_BACKGROUND))
    {
        nk_layout_row_dynamic(ctx, row, 1);
        nk_label(ctx, "Viewport mode:", NK_TEXT_LEFT);
        const char *modes[]   = { "HIDDEN",
                                  "FIXED",
                                  "FIXED BEST FIT",
                                  "STRETCH KEEP ASPECT",
                                  "STRETCH" };
        runtime.viewport_mode = nk_combo(ctx,
                                         modes,
                                         NK_LEN(modes),
                                         runtime.viewport_mode,
                                         row,
                                         nk_vec2(200, 200));
    }
    nk_end(ctx);
}
