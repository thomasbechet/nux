#ifndef NUX_LUA_INTERNAL_H
#define NUX_LUA_INTERNAL_H

#include "module.h"

const nux_module_t *nux_lua_module_info(void);
lua_State          *nux_lua_state(void);

#endif
