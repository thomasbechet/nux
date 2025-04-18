#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "nux.h"

#define NU_STDLIB
#include <nulib/nulib.h>
#ifdef NUX_BUILD_WASM3
#include <wasm3.h>
#endif

#define NUX_KEY_MAKE(block, version) ((block) | ((version << 24) & 0xFF000000))
#define NUX_KEY_VERSION(key)         (((key) & 0xFF000000) >> 24)
#define NUX_KEY_TYPE(key)            (((key) & 0xFF000000) >> 24)
#define NUX_KEY_BLOCK(key)           ((key) & 0xFFFFFF)

///
/// +---------+
/// |   NULL  | <-- 0
/// +---------+ <-- 1
/// |  STATIC |
/// |    v    |
/// +---------+ <-- objects_static_head
/// |   ...   |
/// |  UNUSED |
/// |   ...   |
/// +---------+ <-- objects_dynamic_head
/// |    ^    |
/// | DYNAMIC |
/// +---------+ <-- objects_stack_head
/// |  STACK  |
/// |    v    |
/// +---------+ <-- objects_capa
///

struct nux_env
{
    nux_instance_t inst;
    nu_v2u_t       cursor;
    nux_error_t    error;
    nux_c8_t       error_message[256];
};

typedef struct
{
    nux_u32_t key;  // version (4) | type (8) | block (20)
    nux_u32_t next; // for free list
} nux_object_entry_t;

struct nux_instance
{
    void *userdata;

    nu_bool_t running;
    nux_f32_t time;
    nux_u32_t tps;

    nux_u8_t *memory;
    nux_u32_t memory_capa;

    nux_object_entry_t *objects;
    nux_u32_t           objects_capa;
    nux_u32_t           objects_count;
    nux_u32_t           objects_static_head;
    nux_u32_t           objects_dynamic_head;
    nux_u32_t           objects_dynamic_free;

    nux_id_t root_stack;
    nux_id_t first_scene;

    nux_command_t *cmds;
    nu_u32_t       cmds_capa;
    nu_u32_t       cmds_size;

    nu_u32_t buttons[NUX_PLAYER_MAX];
    nu_f32_t axis[NUX_PLAYER_MAX][NUX_AXIS_MAX];

    struct nux_env env;

    struct
    {
        nu_bool_t started;
        nu_u8_t  *buffer;
        nu_u32_t  buffer_size;
#ifdef NUX_BUILD_WASM3
        IM3Environment env;
        IM3Runtime     runtime;
        IM3Module      module;
        IM3Function    start_function;
        IM3Function    update_function;
#endif
    } wasm;
};

nux_id_t nux_stack_push(nux_env_t         env,
                        nux_id_t          stack,
                        nux_object_type_t type,
                        nux_u32_t         size);
nux_id_t nux_pool_new(nux_env_t env,
                      nux_id_t  allocator,
                      nux_u32_t object_size,
                      nux_u32_t object_capa);
nux_id_t nux_pool_add(nux_env_t env, nux_id_t pool, nux_object_type_t type);
void     nux_pool_remove(nux_env_t env, nux_id_t pool, nux_id_t id);

void      nux_object_init_table(nux_instance_t inst, nux_u32_t capa);
void     *nux_object_get(nux_env_t env, nux_id_t id, nux_object_type_t type);
void     *nux_object_get_unchecked(nux_env_t env, nux_id_t id);
nux_u32_t nux_object_get_block_unchecked(nux_env_t env, nux_id_t id);
nux_id_t nux_object_add(nux_env_t env, nux_object_type_t type, nux_u32_t block);
void     nux_object_remove(nux_env_t env, nux_id_t id);

void nux_set_error(nux_env_t env, nux_error_t error);

nux_status_t nux_load_cartridge(nux_env_t       env,
                                nux_id_t        stack,
                                const nux_c8_t *cart,
                                nux_u32_t       n);

nux_status_t nux_wasm_init(nux_instance_t               inst,
                           const nux_instance_config_t *config);
void         nux_wasm_free(nux_instance_t inst);
nux_status_t nux_wasm_load(nux_env_t env, nux_u8_t *buffer, nux_u32_t n);
nux_status_t nux_wasm_start(nux_env_t env);
nux_status_t nux_wasm_update(nux_env_t env);

void     nux_update_scenes(nux_env_t env);
nux_id_t nux_create_node_with_object(nux_env_t         env,
                                     nux_id_t          parent,
                                     nux_object_type_t type);
nux_id_t nux_node_object(nux_env_t env, nux_id_t node);

#endif
