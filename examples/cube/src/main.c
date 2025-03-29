#include <nux.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>
// #define NUX_IMPLEMENTATION
// #include <nuxlib/freecam.h>
// #include <nuxlib/gamepad.h>
// #include <nuxlib/print.h>
// #include <nuxlib/debug.h>

#define SCENE_ARIANE6    3
#define SCENE_INDUSTRIAL 4

void
start (void)
{
    bind_scene(SCENE_ARIANE6);
    node_add(NUX_NULL);

    set_camera_lookat(CAMERA,
                      (const float[]) { 100, 50, 100 },
                      (const float[]) { 0, 0, 0 },
                      (const float[]) { 0, 1, 0 });
    camera_set_perspective(CAMERA, nu_radian(60), 0.1f, 200);

    bind_scene(SCENE);
}

void
update (void)
{
    nu_f32_t  x = nu_sin(0.42);
    nu_char_t buf[64];
    snprintf(buf, sizeof(buf), "%lf", x);
    trace(buf);
    push_camera(MODEL_INDUSTRIAL);
    draw_model(MODEL_ARIANE6);
}
