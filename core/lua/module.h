#ifndef NUX_LUA_MODULE_H
#define NUX_LUA_MODULE_H

#include <base/module.h>

#include <externals/lua/lua.h>
#include <externals/lua/lualib.h>
#include <externals/lua/lauxlib.h>

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_LUA_MODULE_TABLE "M"
#define NUX_LUA_MODULE_RID   "rid"
#define NUX_LUA_ON_CONF      "on_conf"
#define NUX_LUA_ON_LOAD      "on_load"
#define NUX_LUA_ON_UNLOAD    "on_unload"
#define NUX_LUA_ON_RELOAD    "on_reload"
#define NUX_LUA_ON_UPDATE    "on_update"
#define NUX_LUA_ON_EVENT     "on_event"
#define NUX_LUA_CONF_FILE    "conf.lua"
#define NUX_LUA_INIT_FILE    "init.lua"

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

struct nux_lua_t
{
    nux_u32_t     ref;
    nux_ptr_vec_t event_handles;
};

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// lua.c

nux_status_t nux_lua_configure(nux_config_t *config);
nux_status_t nux_lua_dostring(const nux_c8_t *string);
nux_status_t nux_lua_call_module(nux_lua_t      *lua,
                                 const nux_c8_t *name,
                                 nux_u32_t       nargs);

// lua_bindings.c

nux_status_t nux_lua_open_api(void);

// lua_math.c

nux_status_t nux_lua_open_vmath(void);

void     nux_lua_push_vec2(lua_State *L, nux_v2_t v);
nux_v2_t nux_lua_check_vec2(lua_State *L, int index);
void     nux_lua_push_vec3(lua_State *L, nux_v3_t v);
nux_v3_t nux_lua_check_vec3(lua_State *L, int index);
void     nux_lua_push_vec4(lua_State *L, nux_v4_t v);
nux_v4_t nux_lua_check_vec4(lua_State *L, int index);
void     nux_lua_push_q4(lua_State *L, nux_q4_t q);
nux_q4_t nux_lua_check_q4(lua_State *L, int index);
void     nux_lua_push_mat4(lua_State *L, nux_m4_t m);
nux_m4_t nux_lua_check_mat4(lua_State *L, int index);
void     nux_lua_push_hit(lua_State *L, nux_raycast_hit_t hit);

// lua_require.c

nux_status_t nux_lua_open_require(void);

// lua_event.c

nux_status_t nux_lua_open_event(void);

#endif
