#include <nux.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define NU_STDLIB
#include <nulib/nulib.h>

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
nux_platform_inspect (nux_instance_t     inst,
                      const nux_c8_t    *name,
                      nux_u32_t          n,
                      nux_inspect_type_t type,
                      void              *p)
{
}

int
main (int argc, char **argv)
{
    nux_instance_config_t config = {};
    nux_instance_t        inst   = nux_instance_init(&config);
    nux_env_t             env    = nux_instance_init_env(inst);
    nux_create_scene(env, 1, 1024);
    nux_bind_scene(env, 1);
    nux_nid_t n = nux_node_add(env, NUX_NODE_ROOT);
    n           = nux_node_add(env, n);
    nux_camera_add(env, n);
    nux_node_add(env, NUX_NODE_ROOT);
    n = nux_node_add(env, n);
    n = nux_node_add(env, n);
    nux_node_add(env, NUX_NULL);
    {
        nux_nid_t    *iter = NU_NULL;
        nux_nid_t     stack[128];
        nux_object_t *scene
            = nux_instance_get_object(inst, NUX_OBJECT_SCENE, 1);
        nux_scene_node_t *nodes
            = nux_instance_get_memory(inst, scene->scene.nodes);
        nux_nid_t nid;
        while ((nid = nux_scene_iter_dfs(
                    nodes, &iter, stack, NU_ARRAY_SIZE(stack))))
        {
            nux_f32_t p[3];
            nux_node_get_position(env, nid, p);
            printf("iter %u %x\n", nid, nodes[nid].node.mask);
        }
    }
    nux_instance_tick(inst);
    nux_instance_free(inst);
    return 0;
}
