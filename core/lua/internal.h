#ifndef NUX_LUA_INTERNAL_H
#define NUX_LUA_INTERNAL_H

#include <lua/api.h>
#include <physics/api.h>
#include <input/api.h>
#include <scene/api.h>
#include <graphics/api.h>
#include <externals/lua/lua.h>
#include <externals/lua/lualib.h>
#include <externals/lua/lauxlib.h>

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

nux_status_t nux_lua_configure(nux_config_t *config);
nux_status_t nux_lua_dostring(const nux_c8_t *string);
nux_status_t nux_lua_call_module(nux_lua_t      *lua,
                                 const nux_c8_t *name,
                                 nux_u32_t       nargs);

void      nux_lua_push_vec2(lua_State *L, nux_v2_t v);
nux_v2_t  nux_lua_check_vec2(lua_State *L, int index);
void      nux_lua_push_vec2i(lua_State *L, nux_v2i_t v);
nux_v2i_t nux_lua_check_vec2i(lua_State *L, int index);
void      nux_lua_push_vec3(lua_State *L, nux_v3_t v);
nux_v3_t  nux_lua_check_vec3(lua_State *L, int index);
void      nux_lua_push_vec4(lua_State *L, nux_v4_t v);
nux_v4_t  nux_lua_check_vec4(lua_State *L, int index);
void      nux_lua_push_q4(lua_State *L, nux_q4_t q);
nux_q4_t  nux_lua_check_q4(lua_State *L, int index);
void      nux_lua_push_mat4(lua_State *L, nux_m4_t m);
nux_m4_t  nux_lua_check_mat4(lua_State *L, int index);
void      nux_lua_push_box2i(lua_State *L, nux_b2i_t b);
nux_b2i_t nux_lua_check_box2i(lua_State *L, int index);
void      nux_lua_push_hit(lua_State *L, nux_raycast_hit_t hit);

nux_status_t nux_lua_open_api(void);
nux_status_t nux_lua_open_vmath(void);
nux_status_t nux_lua_open_require(void);
nux_status_t nux_lua_open_event(void);

void       nux_lua_module_register(void);
lua_State *nux_lua_state(void);

#endif
