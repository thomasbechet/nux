#ifndef NUX_LUA_H
#define NUX_LUA_H

#include "base/base.h"
#include "externals/lua/lua.h"
#include "externals/lua/lualib.h"
#include "externals/lua/lauxlib.h"

////////////////////////////
///        MACROS        ///
////////////////////////////

#define NUX_TABLE     "nux"
#define NUX_FUNC_CONF "conf"
#define NUX_FUNC_INIT "init"
#define NUX_FUNC_TICK "tick"

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// lua.c

nux_status_t nux_lua_init(nux_ctx_t *ctx);
void         nux_lua_free(nux_ctx_t *ctx);
nux_status_t nux_lua_configure(nux_ctx_t      *ctx,
                               const nux_c8_t *path,
                               nux_config_t   *config);
nux_status_t nux_lua_invoke(nux_ctx_t *ctx, const nux_c8_t *func);

// lua_api_*.c

nux_status_t nux_lua_open_base(nux_ctx_t *ctx);
nux_status_t nux_lua_open_graphics(nux_ctx_t *ctx);
nux_status_t nux_lua_open_ecs(nux_ctx_t *ctx);

#endif
