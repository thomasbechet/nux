#include "internal.h"

static NU_ENUM_MAP(object_type_map,
                   NU_ENUM_NAME(NUX_OBJECT_RAW, "raw"),
                   NU_ENUM_NAME(NUX_OBJECT_MESH, "mesh"),
                   NU_ENUM_NAME(NUX_OBJECT_WASM, "wasm"),
                   NU_ENUM_NAME(NUX_OBJECT_TEXTURE, "texture"),
                   NU_ENUM_NAME(NUX_OBJECT_SCENE, "scene"));

nux_u32_t
nux_malloc (nux_instance_t inst, nux_u32_t n)
{
    if (inst->memhead + n > inst->memcapa)
    {
        return NUX_INVALID_ADDR;
    }
    nu_u32_t p = inst->memhead;
    inst->memhead += n;
    return p;
}
void *
nux_instance_get_memory (nux_instance_t inst, nux_u32_t addr)
{
    // TODO: check boundary
    return inst->mem + addr;
}
