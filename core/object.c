#include "internal.h"

nux_object_t *
nux_validate_object (nux_env_t env, nux_object_type_t type, nux_oid_t oid)
{
    if (oid >= NUX_OBJECT_MAX)
    {
        nux_set_error(env, NUX_ERROR_INVALID_NODE_ID);
        return NU_NULL;
    }
    if (env->inst->objects[oid].type != type)
    {
        nux_set_error(env, NUX_ERROR_INVALID_OBJECT_TYPE);
        return NU_NULL;
    }
    return env->inst->objects + oid;
}

nux_object_t *
nux_instance_get_object (nux_instance_t inst, nux_oid_t oid)
{
    return &inst->objects[oid];
}

nux_status_t
nux_create_scope (nux_env_t env, nux_oid_t oid, nux_u32_t size)
{
    NU_CHECK(nux_validate_object(env, NUX_OBJECT_FREE, oid),
             return NUX_FAILURE);
    // TODO: create scope
    return NUX_SUCCESS;
}
void
nux_rewind_scope (nux_env_t env, nux_oid_t oid)
{
}
void
nux_set_active_scope (nux_env_t env, nux_oid_t oid)
{
    NU_CHECK(nux_validate_object(env, NUX_OBJECT_SCOPE, oid), return);
    env->active_scope = oid;
}

nux_object_t *
nux_object_set (nux_env_t env, nux_oid_t oid, nux_object_type_t type)
{
    NU_CHECK(nux_validate_object(env, NUX_OBJECT_FREE, oid), return NU_NULL);
    if (type == NUX_OBJECT_NULL || type == NUX_OBJECT_FREE)
    {
        nux_set_error(env, NUX_ERROR_INVALID_OBJECT_CREATION);
        return NU_NULL;
    }
    // TODO: uninit resource
    switch (env->inst->objects[oid].type)
    {
        case NUX_OBJECT_SCOPE:
        case NUX_OBJECT_TEXTURE:
        case NUX_OBJECT_MESH:
        case NUX_OBJECT_SPRITESHEET:
            break;
        default:
            break;
    }
    env->inst->objects[oid].type = type;
    env->inst->objects[oid].next = NU_NULL;
    return env->inst->objects + oid;
}
