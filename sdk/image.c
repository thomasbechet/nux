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
image_resize (nu_v2u_t           source_size,
              const nu_byte_t   *source_data,
              gpu_texture_size_t target_size,
              nu_byte_t         *target_data)
{
    nu_u32_t size = gpu_texture_width(target_size);
    if (!stbir_resize_uint8(source_data,
                            source_size.x,
                            source_size.y,
                            0,
                            target_data,
                            size,
                            size,
                            0,
                            4))
    {
        sdk_log(NU_LOG_ERROR, "Failed to resize image");
        return NU_FAILURE;
    }
    return NU_SUCCESS;
}
nu_status_t
cart_write_texture (sdk_project_t     *proj,
                  nu_u32_t           index,
                  gpu_texture_size_t size,
                  const nu_byte_t   *data)
{
    nu_status_t status;
    nu_u32_t    data_size = gpu_texture_memsize(size);
    status                = cart_write_chunk_header(
        proj, CART_CHUNK_TEXTURE, data_size + sizeof(nu_u32_t) * 2);
    NU_CHECK(status, return NU_FAILURE);
    status = cart_write_u32(proj, index);
    NU_CHECK(status, return NU_FAILURE);
    status = cart_write_u32(proj, size);
    NU_CHECK(status, return NU_FAILURE);
    status = cart_write(proj, data, data_size);
    NU_CHECK(status, return NU_FAILURE);
    return NU_SUCCESS;
}

nu_status_t
sdk_image_load (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    NU_CHECK(json_parse_u32(jasset, "index", &asset->image.target_index),
             return NU_FAILURE);
    NU_CHECK(json_parse_u32(jasset, "size", &asset->image.target_size),
             return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
sdk_image_save (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    NU_CHECK(json_write_u32(jasset, "index", asset->image.target_index),
             return NU_FAILURE);
    NU_CHECK(json_write_u32(jasset, "size", asset->image.target_size),
             return NU_FAILURE);
    return NU_SUCCESS;
}
nu_status_t
sdk_image_compile (sdk_project_t *proj, sdk_project_asset_t *asset)
{
    // Load image
    nu_status_t status = NU_SUCCESS;
    nu_i32_t    w, h, n;
    nu_byte_t  *img = stbi_load(asset->source_path, &w, &h, &n, STBI_rgb_alpha);
    if (!img)
    {
        sdk_log(
            NU_LOG_ERROR, "Failed to load image file %s", asset->source_path);
    }
    nu_size_t  data_size = gpu_texture_memsize(GPU_TEXTURE128);
    nu_byte_t *data      = malloc(data_size);
    NU_CHECK(data, goto cleanup0);
    NU_CHECK(image_resize(nu_v2u(w, h), img, GPU_TEXTURE128, data),
             goto cleanup1);

    // Write cart
    NU_CHECK(
        cart_write_texture(
            proj, asset->image.target_index, asset->image.target_size, data),
        goto cleanup1);

cleanup1:
    free(data);
cleanup0:
    stbi_image_free(img);
    return status;
}
