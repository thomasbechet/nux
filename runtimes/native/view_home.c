#include "nuklear/src/nuklear.h"
#include "runtime.h"

void
view_home (struct nk_context *ctx, struct nk_rect bounds)
{
    // Inspector
    runtime_instance_t *instance = runtime_instance();
    const nu_size_t     row      = 30;
    struct nk_rect window_bounds = nk_rect(bounds.x, bounds.y, 300, bounds.h);
    struct nk_rect instance_bounds
        = nk_rect(window_bounds.w,
                  bounds.y,
                  NU_MAX(0, bounds.w - window_bounds.w),
                  bounds.h);
    if (nk_begin(ctx, "Games", window_bounds, 0))
    {
        // Cartridge
        static nu_size_t instance_count = 0;
        for (nu_size_t i = 0; i < instance_count; ++i)
        {
            nk_layout_row_dynamic(ctx, 110, 1);
            nu_char_t buf[64];
            nu_sv_fmt(buf, sizeof(buf), "Instance #%d", i);
            if (nk_group_begin(ctx, buf, NK_WINDOW_TITLE | NK_WINDOW_BORDER))
            {
                nk_layout_row_dynamic(ctx, row, 1);
                nk_button_label(ctx, "Load");
                nk_button_label(ctx, "Save");
                nk_group_end(ctx);
            }
        }
        // Instances
        nk_layout_row_dynamic(ctx, row, 1);
        {
            if (nk_button_symbol_label(
                    ctx, NK_SYMBOL_PLUS, "New instance", NK_TEXT_CENTERED))
            {
                ++instance_count;
            }
        }
        nk_checkbox_label(ctx, "Pause", &instance->pause);
        // Viewport mode
        {
            nk_label(ctx, "Viewport mode:", NK_TEXT_LEFT);
            const nu_char_t *modes[] = { "HIDDEN",
                                         "FIXED",
                                         "FIXED BEST FIT",
                                         "STRETCH KEEP ASPECT",
                                         "STRETCH" };
            instance->viewport_mode  = nk_combo(ctx,
                                               modes,
                                               NK_LEN(modes),
                                               instance->viewport_mode,
                                               row,
                                               nk_vec2(200, 200));
        }
        nk_end(ctx);
    }

    instance->viewport = instance_bounds;
}
