#include "internal.h"

nu_status_t
sdk_wasm_load (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    return NU_SUCCESS;
}
nu_status_t
sdk_wasm_save (sdk_project_asset_t *asset, JSON_Object *jasset)
{
    return NU_SUCCESS;
}
nu_status_t
sdk_wasm_compile (sdk_project_t *proj, sdk_project_asset_t *asset)
{
    // Load wasm buffer
    nu_size_t   size;
    nu_byte_t  *buffer;
    nu_status_t status = NU_SUCCESS;
    if (!nu_load_bytes(nu_sv(asset->source, NU_PATH_MAX), NU_NULL, &size))
    {
        sdk_log(NU_LOG_ERROR, "Failed to load wasm file %s", asset->source);
        return NU_FAILURE;
    }
    buffer = sdk_malloc(size);
    NU_CHECK(buffer, return NU_FAILURE);
    NU_ASSERT(nu_load_bytes(nu_sv(asset->source, NU_PATH_MAX), buffer, &size));

    // Write cart
    nux_cart_entry_t *entry = sdk_begin_entry(proj, asset->id, NUX_OBJECT_WASM);
    status                  = cart_write(proj, buffer, size);
    NU_CHECK(status, goto cleanup0);

cleanup0:
    sdk_free(buffer);
    return status;
}
