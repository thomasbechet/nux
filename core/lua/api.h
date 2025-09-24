#ifndef NUX_LUA_API_H
#define NUX_LUA_API_H

#include <base/api.h>

typedef struct nux_lua_t nux_lua_t;

nux_lua_t *nux_lua_load(nux_arena_t *arena, const nux_c8_t *path);

#endif
