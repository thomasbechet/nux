#include "sdk.h"

#define STBIR_DEBUG

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
// #include <stb/stb_image_resize2.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

nu_status_t
image_resize (nu_v2u_t         source_size,
              const nu_byte_t *source_data,
              nu_u32_t         target_size,
              nu_byte_t       *target_data)
{
    if (source_size.x == source_size.y && source_size.x == target_size)
    {
        // No resize required, simply copy data
        nu_memcpy(target_data, source_data, gfx_texture_memsize(target_size));
        return NU_SUCCESS;
    }
    if (!stbir_resize_uint8(source_data,
                            source_size.x,
                            source_size.y,
                            0,
                            target_data,
                            target_size,
                            target_size,
                            0,
                            4))
    {
        sdk_log(NU_LOG_ERROR, "Failed to resize image");
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
nu_status_t
cart_write_texture (sdk_project_t   *proj,
                    const nu_char_t *name,
                    nu_u32_t         size,
                    const nu_byte_t *data)
{
    cart_chunk_entry_t *entry = sdk_begin_entry(proj, name, CART_CHUNK_TEXTURE);
    nu_status_t         status;
    status = cart_write_u32(proj, size);
    NU_CHECK(status, return NU_FAILURE);
    status = cart_write(proj, data, gfx_texture_memsize(size));
    NU_CHECK(status, return NU_FAILURE);
    return NU_SUCCESS;
}

nu_status_t
sdk_image_load (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    NU_CHECK(json_parse_u32(jasset, "size", &asset->image.target_size),
             return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
sdk_image_save (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    NU_CHECK(json_write_u32(jasset, "size", asset->image.target_size),
             return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
sdk_image_compile (sdk_project_t *proj, sdk_project_asset_t *asset)
{
    const nu_u32_t target_size = asset->image.target_size;

    // Load image
    nu_status_t status = NU_SUCCESS;
    nu_i32_t    w, h, n;
    nu_byte_t  *img = stbi_load(asset->source, &w, &h, &n, STBI_rgb_alpha);
    if (!img)
    {
        sdk_log(NU_LOG_ERROR, "Failed to load image file %s", asset->source);
        return NU_FAILURE;
    }
    nu_size_t  data_size = gfx_texture_memsize(target_size);
    nu_byte_t *data      = sdk_malloc(data_size);
    NU_CHECK(data, goto cleanup0);
    NU_CHECK(image_resize(nu_v2u(w, h), img, target_size, data), goto cleanup1);

    // Write cart
    NU_CHECK(
        cart_write_texture(proj, asset->name, asset->image.target_size, data),
        goto cleanup1);

cleanup1:
    sdk_free(data);
cleanup0:
    stbi_image_free(img);
    return status;
}
