#include "runtime.h"

static struct
{
    nu_char_t       path[NU_PATH_MAX];
    viewport_mode_t viewport_mode;
    union
    {
        nu_f32_t edit_float[32];
    };
    nu_bool_t edit_override[32];
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

        const nu_f32_t ratio = 0.2;
        const nu_u32_t width = 300;

        // Inspector
        {
            // struct nk_rect bounds = nk_rect(0, 0, size.x * ratio, size.y);
            struct nk_rect bounds = nk_rect(0, 0, width, size.y);
            if (nk_begin(ctx, "Inspector", bounds, NK_WINDOW_TITLE))
            {
                nk_layout_row_dynamic(ctx, 20, 1);
                // Viewport mode
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
                // Inspect values
                {
                    nu_size_t        count;
                    inspect_value_t *values = runtime_inspect_values(0, &count);
                    for (nu_size_t i = 0; i < count; ++i)
                    {
                        inspect_value_t *value = values + i;
                        if (nk_tree_push_hashed(ctx,
                                                NK_TREE_TAB,
                                                value->name,
                                                NK_MAXIMIZED,
                                                value->name,
                                                nu_strlen(value->name),
                                                0))
                        {
                            nk_labelf(ctx,
                                      NK_TEXT_LEFT,
                                      "Address: 0x%X",
                                      value->addr);
                            switch (value->type)
                            {
                                case SYS_INSPECT_I32:
                                    nk_labelf(ctx,
                                              NK_TEXT_LEFT,
                                              "Value: %d",
                                              value->value.i32);
                                    break;
                                case SYS_INSPECT_F32: {
                                    nk_labelf(ctx,
                                              NK_TEXT_LEFT,
                                              "Value: %lf",
                                              value->value.f32);
                                    const nu_f32_t ratios[] = { 0.8, 0.2 };
                                    nk_layout_row(
                                        ctx, NK_DYNAMIC, 20, 3, ratios);
                                    nk_property_float(ctx,
                                                      value->name,
                                                      0,
                                                      &app.edit_float[i],
                                                      11000,
                                                      1,
                                                      0.5);
                                    if (nk_button_label(ctx, "Reset"))
                                    {
                                        app.edit_float[i] = value->value.f32;
                                    }
                                }
                                break;
                            }
                            nk_layout_row_static(ctx, 20, 80, 2);
                            nu_bool_t override_button
                                = nk_button_label(ctx, "Override");
                            nk_checkbox_label(
                                ctx, "Force", &app.edit_override[i]);
                            if (app.edit_override[i] || override_button)
                            {
                                value->value.f32 = app.edit_float[i];
                                value->override  = NU_TRUE;
                            }
                            nk_tree_pop(ctx);
                        }
                    }
                }
                nk_end(ctx);
            }
        }

        // Main view
        {
            // nu_b2i_t viewport
            //     = nu_b2i_xywh(size.x * ratio, 0, size.x * (1 - ratio),
            //     size.y);
            nu_b2i_t viewport = nu_b2i_xywh(width, 0, (size.x - width), size.y);
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
