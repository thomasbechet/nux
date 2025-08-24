#ifndef NUX_LUA_INTERNAL_H
#define NUX_LUA_INTERNAL_H

#include <base/internal.h>

#include <externals/lua/lua.h>
#include <externals/lua/lualib.h>
#include <externals/lua/lauxlib.h>

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_TABLE     "nux"
#define NUX_FUNC_CONF "conf"
#define NUX_FUNC_INIT "init"
#define NUX_FUNC_TICK "tick"

////////////////////////////
///        TYPES         ///
////////////////////////////

typedef struct
{
    nux_u32_t dummy;
} nux_lua_t;

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
nux_status_t nux_lua_configure(nux_ctx_t      *ctx,
                               const nux_c8_t *entry_script,
                               nux_config_t   *config);
nux_status_t nux_lua_call_init(nux_ctx_t *ctx);
nux_status_t nux_lua_call_tick(nux_ctx_t *ctx);
nux_status_t nux_lua_dostring(nux_ctx_t *ctx, const nux_c8_t *string);
nux_status_t nux_lua_reload(nux_ctx_t      *ctx,
                            nux_res_t       res,
                            const nux_c8_t *path);

// lua_bindings*.c

nux_status_t nux_lua_open_lua(nux_ctx_t *ctx);
// lua.c

nux_status_t nux_lua_init(nux_ctx_t *ctx);
void         nux_lua_free(nux_ctx_t *ctx);
nux_status_t nux_lua_configure(nux_ctx_t      *ctx,
                               const nux_c8_t *entry_script,
                               nux_config_t   *config);
nux_status_t nux_lua_call_init(nux_ctx_t *ctx);
nux_status_t nux_lua_call_tick(nux_ctx_t *ctx);
nux_status_t nux_lua_dostring(nux_ctx_t *ctx, const nux_c8_t *string);
nux_status_t nux_lua_reload(nux_ctx_t      *ctx,
                            nux_res_t       res,
                            const nux_c8_t *path);

// lua_bindings*.c

nux_status_t nux_lua_open_base(nux_ctx_t *ctx);
nux_status_t nux_lua_open_io(nux_ctx_t *ctx);

#endif
