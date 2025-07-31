#ifndef NUX_LUA_H
#define NUX_LUA_H

#include "base/base.h"
#include "externals/lua/lua.h"

////////////////////////////
///      FUNCTIONS       ///
////////////////////////////

// lua.c

nux_status_t nux_lua_configure(nux_ctx_t *ctx, const nux_config_t *config);
nux_status_t nux_lua_init(nux_ctx_t *ctx);
void         nux_lua_free(nux_ctx_t *ctx);
nux_status_t nux_lua_tick(nux_ctx_t *ctx);

// lua_api.c.inc

nux_status_t nux_lua_register_base(nux_ctx_t *ctx);

#endif
