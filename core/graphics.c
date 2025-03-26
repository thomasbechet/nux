#include "internal.h"

static nux_command_t *
push_command (nux_instance_t inst)
{
    NU_CHECK(inst->cmds_size < inst->cmds_capa, return NU_NULL);
    nux_command_t *cmd = inst->cmds + inst->cmds_size;
    ++inst->cmds_size;
    return cmd;
}

nux_command_t *
nux_instance_get_command_buffer (nux_instance_t inst, nux_u32_t *count)
{
    *count = inst->cmds_size;
    return inst->cmds;
}

nux_u32_t
nux_texture_memsize (nux_u32_t size)
{
    return size * size * 4;
}
nux_u32_t
nux_vertex_memsize (nux_vertex_attribute_t attributes, nux_u32_t count)
{
    nu_u32_t size = 0;
    if (attributes & NUX_VERTEX_POSITION)
    {
        size += NU_V3_SIZE * count;
    }
    if (attributes & NUX_VERTEX_UV)
    {
        size += NU_V2_SIZE * count;
    }
    if (attributes & NUX_VERTEX_COLOR)
    {
        size += NU_V3_SIZE * count;
    }
    return size * sizeof(nu_f32_t);
}
nux_u32_t
nux_vertex_offset (nux_vertex_attribute_t attributes,
                   nux_vertex_attribute_t attribute,
                   nux_u32_t              count)
{
    NU_ASSERT(attribute & attributes);
    nu_u32_t offset = 0;
    if (attributes & NUX_VERTEX_POSITION)
    {
        if (attribute == NUX_VERTEX_POSITION)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    if (attributes & NUX_VERTEX_UV)
    {
        if (attribute == NUX_VERTEX_UV)
        {
            return offset;
        }
        offset += NU_V2_SIZE * count;
    }
    if (attributes & NUX_VERTEX_COLOR)
    {
        if (attribute == NUX_VERTEX_COLOR)
        {
            return offset;
        }
        offset += NU_V3_SIZE * count;
    }
    return offset;
}
