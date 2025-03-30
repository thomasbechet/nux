#include <nux.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>

#define SCENE_ARIANE6    3
#define SCENE_INDUSTRIAL 4
#define SCENE            10

static u32 camera;
static u32 scene;

void
start (void)
{
    create_scene(SCENE, 16);
    bind_scene(SCENE);

    camera = node_add(NU_NULL);
    camera_add(camera);
    camera_set_perspective(camera, nu_radian(60), 0.1f, 200);
    node_set_translation(camera, (const float[]) { 100, 50, 100 });

    node_add_instance(NU_NULL, SCENE_INDUSTRIAL);
    node_add_instance(NU_NULL, SCENE_ARIANE6);
}

void
update (void)
{
    // draw_scene(SCENE_INDUSTRIAL, camera);
}
