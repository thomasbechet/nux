#include <nux.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>
// #define NUX_IMPLEMENTATION
// #include <nuxlib/freecam.h>
// #include <nuxlib/gamepad.h>
// #include <nuxlib/print.h>
// #include <nuxlib/debug.h>

#define MODEL_INDUSTRIAL 4
#define MODEL_ARIANE6    3
#define CAMERA           5

void
start (void)
{
    create_camera(CAMERA);
    set_camera_lookat(CAMERA,
                      (const float[]) { 100, 50, 100 },
                      (const float[]) { 0, 0, 0 },
                      (const float[]) { 0, 1, 0 });
    set_camera_perspective(CAMERA, nu_radian(60), 0.1f, 200);
}

void
update (void)
{
    push_camera(CAMERA);
    draw_model(MODEL_ARIANE6);
}
