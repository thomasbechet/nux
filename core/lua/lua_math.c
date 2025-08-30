#include "internal.h"

typedef enum
{
    NUX_LUA_TYPE_VEC2,
    NUX_LUA_TYPE_VEC3,
    NUX_LUA_TYPE_VEC4,
    NUX_LUA_TYPE_QUAT,
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
        case NUX_LUA_TYPE_QUAT:
            size = sizeof(nux_q4_t);
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
static nux_lua_userdata_t *
check_userdata (lua_State *L, int index, nux_lua_userdata_type_t type)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, index);
    luaL_argcheck(L, u->type == type, index, "invalid lua userdata type");
    return u;
}

void
nux_lua_push_vec2 (lua_State *L, nux_v2_t v)
{
    nux_v2_t *vec = new_userdata(L, NUX_LUA_TYPE_VEC2);
    *vec          = v;
}
nux_v2_t
nux_lua_check_vec2 (lua_State *L, int index)
{
    if (lua_istable(L, index))
    {
        nux_v2_t v;
        lua_geti(L, index, 1);
        v.x = lua_tonumber(L, -1);
        lua_geti(L, index, 2);
        v.y = lua_tonumber(L, -1);
        return v;
    }
    else
    {
        nux_lua_userdata_t *u = check_userdata(L, index, NUX_LUA_TYPE_VEC2);
        return *u->vec2;
    }
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
    if (lua_istable(L, index))
    {
        nux_v3_t v;
        lua_geti(L, index, 1);
        v.x = lua_tonumber(L, -1);
        lua_geti(L, index, 2);
        v.y = lua_tonumber(L, -1);
        lua_geti(L, index, 3);
        v.z = lua_tonumber(L, -1);
        return v;
    }
    else
    {
        nux_lua_userdata_t *u = check_userdata(L, index, NUX_LUA_TYPE_VEC3);
        return *u->vec3;
    }
}
void
nux_lua_push_vec4 (lua_State *L, nux_v4_t v)
{
    nux_v4_t *vec = new_userdata(L, NUX_LUA_TYPE_VEC4);
    *vec          = v;
}
nux_v4_t
nux_lua_check_vec4 (lua_State *L, int index)
{
    if (lua_istable(L, index))
    {
        nux_v4_t v;
        lua_geti(L, index, 1);
        v.x = lua_tonumber(L, -1);
        lua_geti(L, index, 2);
        v.y = lua_tonumber(L, -1);
        lua_geti(L, index, 3);
        v.z = lua_tonumber(L, -1);
        lua_geti(L, index, 4);
        v.w = lua_tonumber(L, -1);
        return v;
    }
    else
    {
        nux_lua_userdata_t *u = check_userdata(L, index, NUX_LUA_TYPE_VEC4);
        return *u->vec4;
    }
}

static int
math_add (lua_State *L)
{
    nux_lua_userdata_t *a   = check_anyuserdata(L, 1);
    nux_lua_userdata_t *ret = NUX_NULL;
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2: {
            nux_lua_userdata_t *b = check_userdata(L, 2, NUX_LUA_TYPE_VEC2);
            nux_v2_t            v = nux_v2_add(*a->vec2, *b->vec2);
            if (lua_gettop(L) == 3)
            {
                *check_userdata(L, 3, NUX_LUA_TYPE_VEC2)->vec2 = v;
                return 0;
            }
            else
            {
                nux_lua_push_vec2(L, v);
                return 1;
            }
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            nux_lua_userdata_t *b = check_userdata(L, 2, NUX_LUA_TYPE_VEC3);
            nux_v3_t            v = nux_v3_add(*a->vec3, *b->vec3);
            if (lua_gettop(L) == 3)
            {
                *check_userdata(L, 3, NUX_LUA_TYPE_VEC3)->vec3 = v;
                return 0;
            }
            else
            {
                nux_lua_push_vec3(L, v);
                return 1;
            }
        }
        break;
        default:
            luaL_error(L, "unsupported userdata");
    }
    return 0;
}
static int
math_sub (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2: {
            nux_lua_userdata_t *b = check_userdata(L, 2, NUX_LUA_TYPE_VEC3);
            nux_v2_t            v = nux_v2_sub(*a->vec2, *b->vec2);
            if (lua_gettop(L) == 3)
            {
                *check_userdata(L, 3, NUX_LUA_TYPE_VEC3)->vec2 = v;
                return 0;
            }
            else
            {
                nux_lua_push_vec2(L, v);
                return 1;
            }
        }
        break;
        case NUX_LUA_TYPE_VEC3: {
            nux_lua_userdata_t *b = check_userdata(L, 2, NUX_LUA_TYPE_VEC3);
            nux_v3_t            v = nux_v3_sub(*a->vec3, *b->vec3);
            if (lua_gettop(L) == 3)
            {
                *check_userdata(L, 3, NUX_LUA_TYPE_VEC3)->vec3 = v;
                return 0;
            }
            else
            {
                nux_lua_push_vec3(L, v);
                return 1;
            }
        }
        break;
        default:
            luaL_error(L, "unsupported userdata");
    }
    return 0;
}
static int
math_mul (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
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
        default:
            luaL_error(L, "unsupported userdata");
    }
    return 0;
}
static int
math_div (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
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
        default:
            luaL_error(L, "unsupported userdata");
    }
    return 0;
}
static int
math_unm (lua_State *L)
{
    nux_lua_userdata_t *a = check_anyuserdata(L, 1);
    switch (a->type)
    {
        case NUX_LUA_TYPE_VEC2:
            nux_lua_push_vec2(L, nux_v2_muls(*a->vec2, -1));
            return 1;
        case NUX_LUA_TYPE_VEC3:
            nux_lua_push_vec3(L, nux_v3_muls(*a->vec3, -1));
            return 1;
        default:
            luaL_error(L, "unsupported userdata");
    }
    return 0;
}
static int
math_vec2 (lua_State *L)
{
    nux_v2_t v;
    if (lua_gettop(L) == 0)
    {
        v = NUX_V2_ZEROS;
    }
    else if (lua_gettop(L) == 1)
    {
        if (lua_isnumber(L, 1))
        {
            v = nux_v2s(luaL_checknumber(L, 1));
        }
        else
        {
            v = nux_lua_check_vec2(L, 1);
        }
    }
    else
    {
        v.x = luaL_checknumber(L, 1);
        v.y = luaL_checknumber(L, 2);
    }
    nux_lua_push_vec2(L, v);
    return 1;
}
static int
math_vec3 (lua_State *L)
{
    nux_v3_t v;
    if (lua_gettop(L) == 0)
    {
        v = NUX_V3_ZEROS;
    }
    else if (lua_gettop(L) == 1)
    {
        if (lua_isnumber(L, 1))
        {
            v = nux_v3s(luaL_checknumber(L, 1));
        }
        else
        {
            v = nux_lua_check_vec3(L, 1);
        }
    }
    else
    {
        v.x = luaL_checknumber(L, 1);
        v.y = luaL_checknumber(L, 2);
        v.z = luaL_checknumber(L, 3);
    }
    nux_lua_push_vec3(L, v);
    return 1;
}
static int
math_vec4 (lua_State *L)
{
    nux_v4_t v;
    if (lua_gettop(L) == 0)
    {
        v = NUX_V4_ZEROS;
    }
    else if (lua_gettop(L) == 1)
    {
        if (lua_isnumber(L, 1))
        {
            v = nux_v4s(luaL_checknumber(L, 1));
        }
        else
        {
            v = nux_lua_check_vec4(L, 1);
        }
    }
    else
    {
        v.x = luaL_checknumber(L, 1);
        v.y = luaL_checknumber(L, 2);
        v.z = luaL_checknumber(L, 3);
        v.w = luaL_checknumber(L, 4);
    }
    nux_lua_push_vec4(L, v);
    return 1;
}
static int
math_dot (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC2:
            lua_pushnumber(L, nux_v2_dot(*u->vec2, nux_lua_check_vec2(L, 2)));
            break;
        case NUX_LUA_TYPE_VEC3:
            lua_pushnumber(L, nux_v3_dot(*u->vec3, nux_lua_check_vec3(L, 2)));
            break;
        case NUX_LUA_TYPE_VEC4:
            lua_pushnumber(L, nux_v4_dot(*u->vec4, nux_lua_check_vec4(L, 2)));
            break;
        default:
            luaL_error(L, "unsupported type");
            break;
    }
    return 1;
}
static int
math_cross (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC3: {
            nux_v3_t v = nux_v3_cross(*u->vec3, nux_lua_check_vec3(L, 2));
            if (lua_gettop(L) == 3)
            {
                *check_userdata(L, 3, NUX_LUA_TYPE_VEC3)->vec3 = v;
                return 0;
            }
            else
            {
                nux_lua_push_vec3(L, v);
                return 1;
            }
        }
        break;
        default:
            luaL_error(L, "unsupported type");
            break;
    }
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
        case NUX_LUA_TYPE_VEC4:
            lua_pushnumber(L, nux_v4_norm(*u->vec4));
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
        default:
            luaL_error(L, "unsupported userdata");
    }
    return 0;
}
static int
meta_newindex (lua_State *L)
{
    nux_lua_userdata_t *u = check_anyuserdata(L, 1);
    switch (u->type)
    {
        case NUX_LUA_TYPE_VEC2: {
            const char *key = luaL_checkstring(L, 2);
            if (key[0] == 'x')
            {
                u->vec2->x = luaL_checknumber(L, 3);
            }
            else if (key[0] == 'y')
            {
                u->vec2->y = luaL_checknumber(L, 3);
            }
        }
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
        case NUX_LUA_TYPE_VEC4: {
            const char *key = luaL_checkstring(L, 2);
            if (key[0] == 'x')
            {
                u->vec4->x = luaL_checknumber(L, 3);
            }
            else if (key[0] == 'y')
            {
                u->vec4->y = luaL_checknumber(L, 3);
            }
            else if (key[0] == 'z')
            {
                u->vec4->z = luaL_checknumber(L, 3);
            }
            else if (key[0] == 'w')
            {
                u->vec4->w = luaL_checknumber(L, 3);
            }
        }
        break;
        default:
            luaL_error(L, "unsupported userdata");
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
        default:
            break;
    }
    lua_pushnil(L);
    return 1;
}

static int
register_metatable (lua_State *L)
{
    luaL_Reg reg[] = {
        { "__index", meta_index },
        { "__newindex", meta_newindex },
        { "__tostring", meta_tostring },
        { "__add", math_add },
        { "__sub", math_sub },
        { "__mul", math_mul },
        { "__div", math_div },
        { "__unm", math_unm },
        { NULL, NULL },
    };
    luaL_newmetatable(L, "userdata");
    luaL_setfuncs(L, reg, 0);
    return 1;
}

nux_status_t
nux_lua_open_vmath (nux_ctx_t *ctx)
{
    static const struct luaL_Reg vmath_lib[]
        = { { "vec2", math_vec2 },   { "vec3", math_vec3 },
            { "vec4", math_vec4 },   { "dot", math_dot },
            { "cross", math_cross }, { "length", math_length },
            { "add", math_add },     { "sub", math_sub },
            { "mul", math_mul },     { "div", math_div },
            { NULL, NULL } };
    lua_State *L = ctx->lua->L;
    register_metatable(L);
    lua_getglobal(L, "nux");
    lua_newtable(L);
    luaL_setfuncs(L, vmath_lib, 0);
    lua_setfield(L, -2, "vmath");
    lua_pop(L, 1);
    return NUX_SUCCESS;
}
