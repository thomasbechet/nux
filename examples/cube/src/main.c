#include <nux.h>
#define NU_STDLIB
#include <nulib/nulib.h>

#define SCENE_ARIANE6    3
#define SCENE_INDUSTRIAL 4

static u32 camera;

void
start (void)
{
    bind_scene(SCENE_INDUSTRIAL);
    camera = node_add(NU_NULL);
    camera_add(camera);
    camera_set_perspective(camera, nu_radian(60), 0.1f, 200);
    node_set_translation(camera, (const float[]) { 100, 50, 100 });
}

void
update (void)
{
    nu_f32_t  x = nu_sin(0.42);
    nu_char_t buf[64];
    snprintf(buf, sizeof(buf), "%lf", x);
    trace(buf);
    draw_scene(SCENE_INDUSTRIAL, camera);
}
