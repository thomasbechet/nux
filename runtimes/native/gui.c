#include "internal.h"

static const struct
{
    const char *name;
    void (*update)(struct nk_context *ctx, struct nk_rect bounds);
} views[] = { { .name = "Game", .update = view_game },
              { .name = "Open", .update = view_open },
              { .name = "Controls", .update = view_controls },
              { .name = "Settings", .update = view_settings } };

void
gui_update (void)
{
    struct nk_vec2i    size = runtime.size;
    struct nk_context *ctx  = &runtime.nk_glfw.ctx;

    struct nk_rect viewport;
    if (runtime.fullscreen)
    {
        viewport = nk_rect(0, 0, size.x, size.y);
    }
    else
    {
        // Views panel
        const struct nk_rect menu_bounds = nk_rect(0, 0, size.x, 35);
        if (nk_begin(ctx, "main", menu_bounds, NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_template_begin(ctx, 25);
            for (int i = 0; i < (int)ARRAY_LEN(views); ++i)
            {
                // nk_layout_row_template_push_static(ctx, views[i].width);
                nk_layout_row_template_push_static(ctx, 120);
            }
            nk_layout_row_template_push_dynamic(ctx);
            nk_layout_row_template_push_static(ctx, 100);
            nk_layout_row_template_end(ctx);

            for (int i = 0; i < (int)ARRAY_LEN(views); ++i)
            {
                if (nk_button_symbol_label(ctx,
                                           (runtime.active_view == (view_t)i)
                                               ? NK_SYMBOL_CIRCLE_SOLID
                                               : NK_SYMBOL_CIRCLE_OUTLINE,
                                           views[i].name,
                                           NK_TEXT_CENTERED))
                {
                    runtime.active_view = i;
                }
            }

            nk_spacer(ctx);
            if (nk_button_label(ctx, "Exit"))
            {
                runtime.running = true;
            }

            nk_end(ctx);
        }

        viewport
            = nk_rect(0, menu_bounds.h, menu_bounds.w, size.y - menu_bounds.h);
    }

    // Active view
    views[runtime.active_view].update(ctx, viewport);
}
