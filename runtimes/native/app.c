#include "runtime.h"

static struct
{
    nu_char_t       path[NU_PATH_MAX];
    viewport_mode_t viewport_mode;
} app;

static void
init (void)
{
    app.viewport_mode = VIEWPORT_FIXED_BEST_FIT;
    runtime_init_instance(0, nu_sv_cstr(app.path));
}
static void
update (struct nk_context *ctx)
{
    {
        nu_v2u_t size = window_get_size();

        const nu_f32_t ratio = 0.15;

        // Inspector
        {
            struct nk_rect bounds = nk_rect(0, 0, size.x * ratio, size.y);
            if (nk_begin(ctx, "Inspector", bounds, NK_WINDOW_TITLE))
            {
                nk_layout_row_dynamic(ctx, 30, 1);
                {
                    const nu_char_t *modes[] = { "FIXED",
                                                 "FIXED BEST FIT",
                                                 "STRETCH KEEP ASPECT",
                                                 "STRETCH" };
                    app.viewport_mode        = nk_combo(ctx,
                                                 modes,
                                                 NK_LEN(modes),
                                                 app.viewport_mode,
                                                 25,
                                                 nk_vec2(200, 200));
                }
                nk_end(ctx);
            }
        }

        // Main view
        {
            nu_b2i_t viewport
                = nu_b2i_xywh(size.x * ratio, 0, size.x * (1 - ratio), size.y);
            runtime_set_instance_viewport(0, viewport, app.viewport_mode);
        }
    }
}

runtime_app_t
runtime_app_default (nu_sv_t path)
{
    nu_sv_to_cstr(path, app.path, sizeof(app.path));
    return (runtime_app_t) { .init = init, .update = update };
}
