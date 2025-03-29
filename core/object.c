#include "internal.h"

nux_status_t
nux_validate_object (nux_instance_t inst, nux_oid_t oid)
{
    if (oid >= NUX_OBJECT_MAX || oid == NU_NULL)
    {
        nux_set_error(inst, NUX_ERROR_INVALID_ID);
        return NUX_FAILURE;
    }
    return NUX_SUCCESS;
}

nux_object_t *
nux_instance_get_object (nux_instance_t    inst,
                         nux_object_type_t type,
                         nux_oid_t         oid)
{
    NU_CHECK(nux_validate_object(inst, oid), return NU_NULL);
    return inst->objects[oid].type == type ? &inst->objects[oid] : NU_NULL;
}

nux_status_t
nux_create_scope (nux_env_t env, nux_oid_t oid, nux_u32_t size)
{
    NU_CHECK(nux_validate_object(env->inst, oid), return NUX_FAILURE);
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
    NU_CHECK(nux_instance_get_object(env->inst, NUX_OBJECT_SCOPE, oid), return);
    env->active_scope = oid;
}

nux_object_t *
nux_object_set (nux_instance_t inst, nux_oid_t oid, nux_object_type_t type)
{
    NU_CHECK(nux_validate_object(inst, oid) && type != NUX_OBJECT_NULL,
             return NU_NULL);
    // TODO: uninit resource
    switch (inst->objects[oid].type)
    {
        case NUX_OBJECT_SCOPE:
        case NUX_OBJECT_TEXTURE:
        case NUX_OBJECT_MESH:
        case NUX_OBJECT_SPRITESHEET:
            break;
        default:
            break;
    }
    inst->objects[oid].type = type;
    inst->objects[oid].next = NU_NULL;
    return inst->objects + oid;
}
