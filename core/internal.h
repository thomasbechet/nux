#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#include <nulib/nulib.h>
#ifdef NUX_BUILD_WASM3
#include <wasm3.h>
#endif

#define NUX_COMMAND_SIZE 64

#define NUX_CART_HEADER_SIZE      sizeof(nux_cart_header_t)
#define NUX_CART_CHUNK_ENTRY_SIZE (sizeof(nu_u32_t) * 4)
#define NUX_INVALID_ADDR          (nu_u32_t)(0xFFFFFFFF)

struct nux_instance
{
    void *userdata;

    nux_object_t objects[NUX_OBJECT_MAX];

    nu_bool_t   running;
    nux_f32_t   time;
    nux_u32_t   tps;
    nux_error_t error;

    nu_u8_t *mem;
    nu_u32_t memcapa;
    nu_u32_t memhead;

    nux_command_t *cmds;
    nu_u32_t       cmds_capa;
    nu_u32_t       cmds_size;

    nu_u32_t buttons[NUX_PLAYER_MAX];
    nu_f32_t axis[NUX_PLAYER_MAX][NUX_AXIS_MAX];

    struct
    {
        nu_bool_t loaded;
#ifdef NUX_BUILD_WASM3
        IM3Environment env;
        IM3Runtime     runtime;
        IM3Module      module;
        IM3Function    start_function;
        IM3Function    update_function;
#endif
    } wasm;
};

struct nux_env
{
    nux_instance_t inst;
    nux_oid_t      active_scope;
    nux_oid_t      active_scene;
    nu_v2u_t       cursor;
};

nu_u32_t nux_malloc(nux_instance_t inst, nux_u32_t n);

struct nux_env nux_env_init(nux_instance_t inst, nux_oid_t scene);
nu_bool_t      nux_validate_object(nux_oid_t oid);
nu_bool_t      nux_validate_node(nux_nid_t nid);

nu_status_t nux_wasm_init(nux_instance_t               inst,
                          const nux_instance_config_t *config);
void        nux_wasm_free(nux_instance_t inst);
nu_status_t nux_wasm_load(nux_instance_t  inst,
                          const nux_u8_t *data,
                          nux_u32_t       n);
nu_status_t nux_wasm_start(nux_instance_t inst);
nu_status_t nux_wasm_update(nux_instance_t inst);

nux_object_t *nux_object_set(nux_instance_t    inst,
                             nux_oid_t         oid,
                             nux_object_type_t type);

nux_error_t      nux_node_add_component(nux_env_t            env,
                                        nux_nid_t            node,
                                        nux_component_type_t component);
nux_error_t      nux_node_remove_component(nux_env_t            env,
                                           nux_nid_t            node,
                                           nux_component_type_t component);
nux_component_t *nux_node_get_component(nux_env_t            env,
                                        nux_nid_t            node,
                                        nux_component_type_t component);

#endif
