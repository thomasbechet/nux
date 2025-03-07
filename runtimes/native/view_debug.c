#include "runtime.h"

static struct
{
    nu_bool_t initialized;
    union
    {
        nu_f32_t edit_float[32];
    };
    nu_f32_t  edit_step[32];
    nu_f32_t  edit_step_pixel[32];
    nu_bool_t edit_override[32];
} app;

static void
init (void)
{
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(app.edit_override); ++i)
    {
        app.edit_override[i]   = NU_FALSE;
        app.edit_float[i]      = 0;
        app.edit_step[i]       = 1;
        app.edit_step_pixel[i] = 0.5;
    }
    app.initialized = NU_TRUE;
}
void
view_debug (struct nk_context *ctx, struct nk_rect bounds)
{
    if (!app.initialized)
    {
        init();
    }

    // Inspector
    const nu_size_t     row      = 30;
    runtime_instance_t *instance = runtime_instance();
    struct nk_rect window_bounds = nk_rect(bounds.x, bounds.y, 300, bounds.h);
    struct nk_rect instance_bounds
        = nk_rect(window_bounds.w,
                  bounds.y,
                  NU_MAX(0, bounds.w - window_bounds.w),
                  bounds.h);
    if (nk_begin(ctx, "Inspector", window_bounds, NK_WINDOW_TITLE))
    {
        // Inspect values
        {
            for (nu_size_t i = 0; i < instance->inspect_value_count; ++i)
            {
                inspect_value_t *value = instance->inspect_values + i;
                if (nk_tree_push_hashed(ctx,
                                        NK_TREE_TAB,
                                        value->name,
                                        NK_MINIMIZED,
                                        value->name,
                                        nu_strlen(value->name),
                                        0))
                {
                    nk_layout_row_dynamic(ctx, row, 2);
                    nk_labelf(ctx, NK_TEXT_LEFT, "Address");
                    nk_labelf(ctx, NK_TEXT_LEFT, "0x%X", value->addr);
                    switch (value->type)
                    {
                        case SYS_INSPECT_I32:
                            nk_labelf(ctx,
                                      NK_TEXT_LEFT,
                                      "Value: %d",
                                      value->value.i32);
                            break;
                        case SYS_INSPECT_F32: {
                            nk_labelf(ctx, NK_TEXT_LEFT, "Value");
                            nk_labelf(
                                ctx, NK_TEXT_LEFT, "%lf", value->value.f32);
                            nk_layout_row_template_begin(ctx, row);
                            nk_layout_row_template_push_dynamic(ctx);
                            nk_layout_row_template_push_static(ctx, row);
                            nk_layout_row_template_push_static(ctx, row);
                            nk_layout_row_template_end(ctx);
                            nk_property_float(ctx,
                                              value->name,
                                              NU_FLT_MIN,
                                              &app.edit_float[i],
                                              NU_FLT_MAX,
                                              app.edit_step[i],
                                              app.edit_step_pixel[i]);
                            nk_button_label(ctx, "+");
                            nk_button_label(ctx, "-");
                        }
                        break;
                    }
                    nk_layout_row_dynamic(ctx, row, 3);
                    if (nk_button_label(ctx, "Read"))
                    {
                        app.edit_float[i] = value->value.f32;
                    }
                    nu_bool_t override_button = nk_button_label(ctx, "Write");
                    nk_checkbox_label(ctx, "Override", &app.edit_override[i]);
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

    instance->viewport = instance_bounds;
}
