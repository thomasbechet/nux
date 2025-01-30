#include "sdk.h"

static sdk_log_callback_t log_callback;

void
sdk_set_log_callback (sdk_log_callback_t callback)
{
    log_callback = callback;
}

void
sdk_vlog (nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    if (log_callback)
    {
        log_callback(level, fmt, args);
    }
}
void
sdk_log (nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    sdk_vlog(level, fmt, args);
    va_end(args);
}

nu_u32_t
sdk_next_mesh_index (sdk_project_t *proj)
{
    nu_u32_t index = proj->next_mesh_index++;
    return index;
}
nu_u32_t
sdk_next_texture_index (sdk_project_t *proj)
{
    nu_u32_t  index = proj->next_texture_index++;
    nu_bool_t found = NU_TRUE;
    while (found)
    {
        found = NU_FALSE;
        for (nu_size_t i = 0; i < proj->assets_count; ++i)
        {
            switch (proj->assets[i].type)
            {
                case SDK_ASSET_IMAGE:
                    if (proj->assets[i].image.target_index == index)
                    {
                        break;
                        found = NU_TRUE;
                    }
                    break;
                default:
                    break;
            }
        }
        if (found)
        {
            index = proj->next_texture_index++;
        }
    }
    return index;
}
nu_u32_t
sdk_next_model_index (sdk_project_t *proj)
{
    nu_u32_t index = proj->next_model_index++;
    return index;
}

nu_status_t
json_parse_f32 (const JSON_Object *object, const nu_char_t *name, nu_f32_t *v)
{
    NU_ASSERT(object && name);
    *v = json_object_get_number(object, name);
    return NU_SUCCESS;
}
nu_status_t
json_write_f32 (JSON_Object *object, const nu_char_t *name, nu_f32_t value)
{
    json_object_set_number(object, name, value);
    return NU_SUCCESS;
}
nu_status_t
json_parse_u32 (const JSON_Object *object, const nu_char_t *name, nu_u32_t *v)
{
    nu_f32_t value;
    NU_CHECK(json_parse_f32(object, name, &value), return NU_FAILURE);
    *v = value;
    return NU_SUCCESS;
}
nu_status_t
json_write_u32 (JSON_Object *object, const nu_char_t *name, nu_u32_t value)
{
    json_write_f32(object, name, value);
    return NU_SUCCESS;
}

nu_status_t
cart_write (sdk_project_t *proj, const void *p, nu_size_t n)
{
    NU_CHECK(fwrite(p, n, 1, proj->f) == 1, return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
cart_write_u32 (sdk_project_t *proj, nu_u32_t v)
{
    nu_u32_t a = nu_u32_le(v);
    NU_CHECK(fwrite(&a, sizeof(a), 1, proj->f) == 1, return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
cart_write_f32 (sdk_project_t *proj, nu_f32_t v)
{
    nu_u32_t value;
    nu_memcpy(&value, &v, sizeof(nu_f32_t));
    return cart_write_u32(proj, value);
}
nu_status_t
cart_write_v2 (sdk_project_t *proj, nu_v2_t v)
{
    for (nu_size_t i = 0; i < NU_V2_SIZE; ++i)
    {
        NU_CHECK(cart_write_f32(proj, v.data[i]), return NU_FAILURE);
    }
    return NU_SUCCESS;
}
nu_status_t
cart_write_v3 (sdk_project_t *proj, nu_v3_t v)
{
    for (nu_size_t i = 0; i < NU_V3_SIZE; ++i)
    {
        NU_CHECK(cart_write_f32(proj, v.data[i]), return NU_FAILURE);
    }
    return NU_SUCCESS;
}
nu_status_t
cart_write_m4 (sdk_project_t *proj, nu_m4_t v)
{
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        NU_CHECK(cart_write_f32(proj, v.data[i]), return NU_FAILURE);
    }
    return NU_SUCCESS;
}
nu_status_t
cart_write_chunk_header (sdk_project_t    *proj,
                         cart_chunk_type_t type,
                         nu_u32_t          length)
{
    NU_CHECK(cart_write_u32(proj, type), return NU_FAILURE);
    NU_CHECK(cart_write_u32(proj, length), return NU_FAILURE);
    return NU_SUCCESS;
}
