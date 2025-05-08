#include "internal.h"

static struct
{
    nu_size_t active_view;
} gui;

static const struct
{
    const nu_char_t *name;
    void (*update)(struct nk_context *ctx, struct nk_rect bounds);
} views[] = { { .name = "Home", .update = view_home },
              { .name = "Controls", .update = view_controls },
              { .name = "Settings", .update = view_settings },
#ifdef NUX_BUILD_SDK
              { .name = "Debug", .update = view_debug }
#endif
};

void
gui_update (void)
{
    // Cartridge / Run
    // - Open cartridge
    // - Open cartridge network
    // - Reset cartridge
    // - Save state
    // - Load state
    // Control window
    // - Input mapping
    // System window
    // - Network config
    // Debug window
    // - Inspector
    // - Resource viewer
    // - Resource hotreload
    // Editor window
    // - Load project
    // - Build project
    // - Resource assembly

    nu_v2u_t           size = window_get_size();
    struct nk_context *ctx  = window_nk_context();

    // Views panel
    const struct nk_rect menu_bounds = nk_rect(0, 0, size.x, 40);
    if (nk_begin(ctx, "main", menu_bounds, NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_template_begin(ctx, 30);
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(views); ++i)
        {
            nk_layout_row_template_push_static(ctx, 130);
        }
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_push_static(ctx, 100);
        nk_layout_row_template_end(ctx);

        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(views); ++i)
        {
            if (nk_button_symbol_label(ctx,
                                       (gui.active_view == i)
                                           ? NK_SYMBOL_CIRCLE_SOLID
                                           : NK_SYMBOL_CIRCLE_OUTLINE,
                                       views[i].name,
                                       NK_TEXT_CENTERED))
            {
                gui.active_view = i;
            }
        }

        nk_spacer(ctx);
        if (nk_button_label(ctx, "Exit"))
        {
            runtime_quit();
        }

        nk_end(ctx);
    }

    // Active view
    struct nk_rect viewport
        = nk_rect(0, menu_bounds.h, menu_bounds.w, size.y - menu_bounds.h);
    views[gui.active_view].update(ctx, viewport);
}
