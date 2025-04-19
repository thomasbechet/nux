#include "internal.h"

static struct
{
    FILE *file;
} io;

static nu_b2i_t
apply_viewport_mode (nu_b2i_t viewport, viewport_mode_t mode)
{
    nu_v2_t global_pos  = nu_v2(viewport.min.x, viewport.min.y);
    nu_v2_t global_size = nu_v2_v2u(nu_b2i_size(viewport));

    nu_v2_t  screen       = nu_v2(NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT);
    nu_f32_t aspect_ratio = screen.x / screen.y;

    nu_v2_t vsize = NU_V2_ZEROS;
    switch (mode)
    {
        case VIEWPORT_FIXED: {
            vsize = nu_v2(screen.x, screen.y);
        };
        break;
        case VIEWPORT_FIXED_BEST_FIT: {
            nu_f32_t w_factor = global_size.x / screen.x;
            nu_f32_t h_factor = global_size.y / screen.y;
            nu_f32_t min      = NU_MIN(w_factor, h_factor);
            if (min < 1)
            {
                // 0.623 => 0.5
                // 0,432 => 0.25
                // 0.115 => 0,125
                nu_f32_t n = 2;
                while (min < (1 / n))
                {
                    n *= 2;
                }
                min = 1 / n;
            }
            else
            {
                min = nu_floor(min);
            }
            vsize.x = screen.x * min;
            vsize.y = screen.y * min;
        }
        break;
        case VIEWPORT_STRETCH_KEEP_ASPECT: {
            if (global_size.x / global_size.y >= aspect_ratio)
            {
                vsize.x = nu_floor(global_size.y * aspect_ratio);
                vsize.y = nu_floor(global_size.y);
            }
            else
            {
                vsize.x = nu_floor(global_size.x);
                vsize.y = nu_floor(global_size.x / aspect_ratio);
            }
        }
        break;
        case VIEWPORT_STRETCH:
            vsize = global_size;
            break;
        default:
            break;
    }

    nu_v2_t vpos = nu_v2_sub(global_size, vsize);
    vpos         = nu_v2_divs(vpos, 2);
    vpos         = nu_v2_add(vpos, global_pos);
    return nu_b2i_xywh(vpos.x, vpos.y, vsize.x, vsize.y);
}

void *
nux_platform_malloc (void *userdata, nux_memory_usage_t usage, nux_u32_t n)
{
    return malloc(n);
}
void
nux_platform_free (void *userdata, void *p)
{
    free(p);
}
void
nux_platform_log (nux_instance_t inst, const nux_c8_t *log, nux_u32_t n)
{
    printf("%.*s\n", (int)n, log);
}
void
nux_platform_debug (nux_instance_t   inst,
                    const nux_c8_t  *name,
                    nux_u32_t        n,
                    nux_debug_type_t type,
                    void            *p)
{
}
nux_status_t
nux_platform_mount (nux_instance_t inst, const nux_c8_t *name, nux_u32_t n)
{
    io.file = fopen((char *)name, "rb");
    return io.file ? NUX_SUCCESS : NUX_FAILURE;
}
nux_status_t
nux_platform_seek (nux_instance_t inst, nux_u32_t n)
{
    fseek(io.file, n, SEEK_SET);
    return NUX_SUCCESS;
}
nux_u32_t
nux_platform_read (nux_instance_t inst, void *p, nux_u32_t n)
{
    NU_CHECK(fread(p, n, 1, io.file) == 1, return 0);
    return n;
}

int
main (int argc, char **argv)
{
    nux_instance_config_t config = {
        .init   = loop_init,
        .update = loop_update,
    };
    nux_instance_t instance = nux_instance_init(&config);

    // Initialize runtime
    nu_status_t status;
    status = window_init();
    NU_CHECK(status, goto cleanup0);
    status = renderer_init();
    NU_CHECK(status, goto cleanup1);

    // Main loop
    nu_bool_t running = NU_TRUE;
    nu_u32_t  fps     = 0;
    while (running)
    {
        // Retrieve window events
        window_poll_events();
        window_update_inputs(instance);

        // Update instance
        nux_instance_set_stat(instance, NUX_STAT_FPS, fps);
        nux_instance_tick(instance);

        // Clear window
        nu_v2u_t size = window_get_size();
        renderer_clear(nu_b2i_xywh(0, 0, size.x, size.y), size);

        nu_b2i_t viewport = nu_b2i_xywh(0, 0, size.x, size.y);
        // viewport = apply_viewport_mode(viewport, VIEWPORT_FIXED_BEST_FIT);
        viewport = apply_viewport_mode(viewport, VIEWPORT_STRETCH_KEEP_ASPECT);
        renderer_render_instance(instance, viewport, size);

        // Swap buffers
        fps = window_swap_buffers();

        // Process runtime events
        runtime_command_t cmd;
        while (window_poll_command(&cmd))
        {
            switch (cmd)
            {
                case COMMAND_EXIT:
                    running = NU_FALSE;
                    break;
                case COMMAND_SAVE_STATE:
                    break;
                case COMMAND_LOAD_STATE:
                    break;
            }
        }
    }

    renderer_free();
cleanup1:
    window_free();
cleanup0:
    nux_instance_free(instance);
    return status;
}
