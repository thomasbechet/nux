#include "internal.h"

typedef enum
{
    NUX_LUA_TYPE_VEC2,
    NUX_LUA_TYPE_VEC3,
    NUX_LUA_TYPE_VEC4,
} nux_lua_userdata_type_t;

typedef struct
{
    nux_lua_userdata_type_t type;
    union
    {
        void     *data;
        nux_v2_t *vec2;
        nux_v3_t *vec3;
        nux_v4_t *vec4;
        nux_q4_t *quat;
    };
} nux_lua_userdata_t;

static void *
new_userdata (lua_State *L, nux_lua_userdata_type_t type)
{
    nux_u32_t size;
    switch (type)
    {
        case NUX_LUA_TYPE_VEC2:
            size = sizeof(nux_v2_t);
            break;
        case NUX_LUA_TYPE_VEC3:
            size = sizeof(nux_v3_t);
            break;
        case NUX_LUA_TYPE_VEC4:
            size = sizeof(nux_v4_t);
            break;
    }
    nux_lua_userdata_t *u
        = lua_newuserdata(L, sizeof(*u) + size); // TODO: support alignment
    u->type = type;
    u->data = u + 1;
    luaL_setmetatable(L, "userdata");
    return u->data;
}
static nux_lua_userdata_t *
check_anyuserdata (lua_State *L, int index)
{
    if (!lua_isuserdata(L, index))
    {
        luaL_argerror(L, index, "'userdata' expected");
    }
    return lua_touserdata(L, index);
}
static void *
check_userdata (lua_State *L, int index, nux_lua_userdata_type_t type)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, index);
    luaL_argcheck(L, u->type == type, index, "invalid lua userdata type");
    return u;
}

void
nux_lua_push_vec3 (lua_State *L, nux_v3_t v)
{
    nux_v3_t *vec = new_userdata(L, NUX_LUA_TYPE_VEC3);
    *vec          = v;
}
nux_v3_t
nux_lua_check_vec3 (lua_State *L, int index)
{
    nux_lua_userdata_t *u = check_userdata(L, index, NUX_LUA_TYPE_VEC3);
    return *u->vec3;
}

static int
vec2_new (lua_State *L)
{
    return 1;
}
static int
vec3_new (lua_State *L)
{
    nux_v3_t v;
    if (lua_gettop(L) == 0)
    {
        v = NUX_V3_ZEROS;
    }
    else if (lua_gettop(L) == 1)
    {
        v = nux_v3s(luaL_checknumber(L, 1));
    }
    else
    {
        v = nux_v3(luaL_checknumber(L, 1),
                   luaL_checknumber(L, 2),
                   luaL_checknumber(L, 3));
    }
    nux_lua_push_vec3(L, v);
    return 1;
}
static int
vec4_new (lua_State *L)
{
    return 1;
}
static int
math_length (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC2:
            lua_pushnumber(L, nux_v2_norm(*u->vec2));
            break;
        case NUX_LUA_TYPE_VEC3:
            lua_pushnumber(L, nux_v3_norm(*u->vec3));
            break;
        default:
            luaL_error(L, "unsupported type");
            break;
    }
    return 1;
}
static int
meta_index (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC2: {
            const char *key = luaL_checkstring(L, 2);
            if (key[0] == 'x')
            {
                lua_pushnumber(L, u->vec2->x);
                return 1;
            }
            else if (key[0] == 'y')
            {
                lua_pushnumber(L, u->vec2->y);
                return 1;
            }
            return 0;
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            const char *key = luaL_checkstring(L, 2);
            if (key[0] == 'x')
            {
                lua_pushnumber(L, u->vec3->x);
                return 1;
            }
            else if (key[0] == 'y')
            {
                lua_pushnumber(L, u->vec3->y);
                return 1;
            }
            else if (key[0] == 'z')
            {
                lua_pushnumber(L, u->vec3->z);
                return 1;
            }
            return 0;
        }
        break;
        case NUX_LUA_TYPE_VEC4: {
        }
        break;
    }
    return 0;
}
static int
meta_newindex (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC2:
            break;
        case NUX_LUA_TYPE_VEC3: {
            const char *key = luaL_checkstring(L, 2);
            if (key[0] == 'x')
            {
                u->vec3->x = luaL_checknumber(L, 3);
            }
            else if (key[0] == 'y')
            {
                u->vec3->y = luaL_checknumber(L, 3);
            }
            else if (key[0] == 'z')
            {
                u->vec3->z = luaL_checknumber(L, 3);
            }
        }
        break;
        case NUX_LUA_TYPE_VEC4:
            break;
    }
    return 0;
}
static int
meta_tostring (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC2:
            lua_pushfstring(L, "vec2(%f, %f)", u->vec2->x, u->vec2->y);
            return 1;
        case NUX_LUA_TYPE_VEC3:
            lua_pushfstring(
                L, "vec3(%f, %f, %f)", u->vec3->x, u->vec3->y, u->vec3->z);
            return 1;
        case NUX_LUA_TYPE_VEC4:
            lua_pushfstring(L,
                            "vec4(%f, %f, %f, %f)",
                            u->vec4->x,
                            u->vec4->y,
                            u->vec4->z,
                            u->vec4->w);
            return 1;
    }
    return 0;
}
static int
meta_add (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2: {
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            nux_lua_userdata_t *b = check_userdata(L, 2, NUX_LUA_TYPE_VEC3);
            nux_lua_push_vec3(L, nux_v3_add(*a->vec3, *b->vec3));
            return 1;
        }
        break;
        case NUX_LUA_TYPE_VEC4: {
        }
        break;
    }
    return 0;
}
static int
meta_sub (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2: {
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            nux_lua_userdata_t *b = check_userdata(L, 2, NUX_LUA_TYPE_VEC3);
            nux_lua_push_vec3(L, nux_v3_sub(*a->vec3, *b->vec3));
            return 1;
        }
        break;
        case NUX_LUA_TYPE_VEC4: {
        }
        break;
    }
    return 0;
}
static int
meta_mul (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2: {
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            if (lua_isuserdata(L, 2))
            {
                nux_lua_userdata_t *b = check_anyuserdata(L, 2);
                if (b->type == NUX_LUA_TYPE_VEC3)
                {
                    nux_lua_push_vec3(L, nux_v3_mul(*a->vec3, *b->vec3));
                    return 1;
                }
            }
            else if (lua_isnumber(L, 2))
            {
                nux_lua_push_vec3(L, nux_v3_muls(*a->vec3, lua_tonumber(L, 2)));
                return 1;
            }
        }
        break;
        case NUX_LUA_TYPE_VEC4: {
        }
        break;
    }
    return 0;
}
static int
meta_div (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2: {
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            if (lua_isuserdata(L, 2))
            {
                nux_lua_userdata_t *b = check_anyuserdata(L, 2);
                if (b->type == NUX_LUA_TYPE_VEC3)
                {
                    nux_lua_push_vec3(L, nux_v3_div(*a->vec3, *b->vec3));
                    return 1;
                }
            }
            else if (lua_isnumber(L, 2))
            {
                nux_lua_push_vec3(L, nux_v3_divs(*a->vec3, lua_tonumber(L, 2)));
                return 1;
            }
        }
        break;
        case NUX_LUA_TYPE_VEC4: {
        }
        break;
    }
    return 0;
}

static int
register_metatable (lua_State *L)
{
    luaL_Reg reg[] = {
        { "__index", meta_index },       { "__newindex", meta_newindex },
        { "__tostring", meta_tostring }, { "__add", meta_add },
        { "__sub", meta_sub },           { "__mul", meta_mul },
        { "__div", meta_div },           { NULL, NULL },
    };

    luaL_newmetatable(L, "userdata");
    luaL_setfuncs(L, reg, 0);

    return 1;
}

nux_status_t
nux_lua_open_math (nux_ctx_t *ctx)
{
    static const struct luaL_Reg math_lib[] = { { "vec2", vec2_new },
                                                { "vec3", vec3_new },
                                                { "vec4", vec4_new },
                                                { "length", math_length },
                                                { NULL, NULL } };

    lua_State *L = ctx->lua->L;
    register_metatable(L);
    lua_getglobal(L, "nux");
    luaL_setfuncs(L, math_lib, 0);
    lua_pop(L, 1);
    return NUX_SUCCESS;
}
