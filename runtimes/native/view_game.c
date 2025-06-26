#include "internal.h"

void
view_game (struct nk_context *ctx, struct nk_rect bounds)
{
    instance_t *instance = runtime_instance();
    const int   row      = 30;
    // if (nk_begin(ctx, "Games", left_bounds, NK_WINDOW_NO_SCROLLBAR))
    // {
    //     nk_layout_row_dynamic(ctx, row, 1);
    //
    //     nk_label(ctx, "Cartridge:", NK_TEXT_ALIGN_LEFT |
    //     NK_TEXT_ALIGN_MIDDLE);
    //
    //     if (instance->active)
    //     {
    //         nk_label(
    //             ctx, "Actions:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
    //
    //         if (nk_button_label(ctx, instance->pause ? "Resume" : "Pause"))
    //         {
    //             instance->pause = !instance->pause;
    //         }
    //         if (nk_button_label(ctx, "Reset"))
    //         {
    //             runtime_reset(0);
    //         }
    //         if (nk_button_label(ctx, "Close"))
    //         {
    //             runtime_close(0);
    //         }
    //
    //         nk_layout_row_dynamic(ctx, row, 1);
    //
    //         // Viewport mode
    //     }
    // }
    // nk_end(ctx);

    if (!runtime_instance()->active)
    {
        if (nk_begin(ctx,
                     "Welcome",
                     bounds,
                     NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR
                         | NK_WINDOW_BACKGROUND))
        {
            nk_layout_row_static(ctx, bounds.h / 2 - row, 0, 0);
            nk_layout_row_dynamic(ctx, row, 1);
            nk_label(ctx,
                     "No active cartridge",
                     NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED);
            nk_layout_row_template_begin(ctx, row);
            nk_layout_row_template_push_dynamic(ctx);
            nk_layout_row_template_push_static(ctx, 100);
            nk_layout_row_template_push_dynamic(ctx);
            nk_layout_row_template_end(ctx);
            nk_spacer(ctx);
            if (nk_button_label(ctx, "Open File"))
            {
                command_push((command_t) { .type = COMMAND_CHANGE_VIEW,
                                           .view = VIEW_OPEN });
            }
            nk_spacer(ctx);
        }
        nk_end(ctx);
    }

    instance->viewport_ui = bounds;
}
