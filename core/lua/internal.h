#ifndef NUX_LUA_INTERNAL_H
#define NUX_LUA_INTERNAL_H

#include <base/internal.h>

#include <externals/lua/lua.h>
#include <externals/lua/lualib.h>
#include <externals/lua/lauxlib.h>

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_MODULE_TABLE "M"
#define NUX_FUNC_CONF    "on_conf"
#define NUX_FUNC_LOAD    "on_load"
#define NUX_FUNC_UNLOAD  "on_unload"
#define NUX_FUNC_RELOAD  "on_reload"
#define NUX_FUNC_UPDATE  "on_update"
#define NUX_FUNC_EVENT   "on_event"
#define NUX_CONF_FILE    "conf.lua"
#define NUX_INIT_FILE    "init.lua"

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef struct
{
    lua_State      *L;
    nux_b32_t       serialize;
    const nux_c8_t *stack[8];
    nux_u32_t       head;
} nux_lua_serde_t;

typedef struct
{
    const nux_c8_t *name;
    nux_u32_t       value;
} nux_lua_serde_enum_t;

typedef struct
{
    nux_u32_t ref;
} nux_lua_t;

typedef struct nux_lua_module
{
    lua_State *L;
} nux_lua_module_t;

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// lua.c

nux_status_t nux_lua_init(nux_ctx_t *ctx);
void         nux_lua_free(nux_ctx_t *ctx);
nux_status_t nux_lua_configure(nux_ctx_t *ctx, nux_config_t *config);
nux_status_t nux_lua_call_module(nux_ctx_t      *ctx,
                                 nux_rid_t       module,
                                 const nux_c8_t *name);
nux_status_t nux_lua_dostring(nux_ctx_t *ctx, const nux_c8_t *string);
nux_status_t nux_lua_reload(nux_ctx_t      *ctx,
                            nux_rid_t       rid,
                            const nux_c8_t *path);

// lua_bindings.c

nux_status_t nux_lua_open_api(nux_ctx_t *ctx);

// lua_math.c

nux_status_t nux_lua_open_vmath(nux_ctx_t *ctx);
void         nux_lua_push_vec2(lua_State *L, nux_v2_t v);
nux_v2_t     nux_lua_check_vec2(lua_State *L, int index);
void         nux_lua_push_vec3(lua_State *L, nux_v3_t v);
nux_v3_t     nux_lua_check_vec3(lua_State *L, int index);
void         nux_lua_push_vec4(lua_State *L, nux_v4_t v);
nux_v4_t     nux_lua_check_vec4(lua_State *L, int index);
void         nux_lua_push_mat4(lua_State *L, nux_m4_t m);
nux_m4_t     nux_lua_check_mat4(lua_State *L, int index);
void         nux_lua_push_hit(lua_State *L, nux_raycast_hit_t hit);

// lua_require.c

nux_status_t nux_lua_open_require(nux_ctx_t *ctx);

#endif
