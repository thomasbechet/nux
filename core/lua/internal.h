#ifndef NUX_LUA_INTERNAL_H
#define NUX_LUA_INTERNAL_H

#include "module.h"

typedef struct
{
    lua_State *L;
} nux_lua_module_t;

nux_lua_module_t *nux_lua_module(void);

nux_status_t nux_lua_init(void);
void         nux_lua_free(void);
void         nux_lua_update(void);

#endif
