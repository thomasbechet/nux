#ifndef NUX_INTERNAL_H
#define NUX_INTERNAL_H

#include "sdk.h"

#include <parson/parson.h>

void *sdk_malloc(nu_size_t n);
void  sdk_free(void *p);
void *sdk_realloc(void *p, nu_size_t n);
void  sdk_log(nu_log_level_t level, const nu_char_t *fmt, ...);

// void view_debug(struct nk_context *ctx, struct nk_rect bounds);

nux_cart_entry_t *sdk_begin_entry(sdk_project_t    *proj,
                                  nux_id_t          id,
                                  nux_object_type_t type);
nux_id_t          sdk_next_id(sdk_project_t *proj);

nu_status_t sdk_wasm_load(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_wasm_save(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_wasm_compile(sdk_project_t       *project,
                             sdk_project_asset_t *asset);

nu_status_t sdk_texture_load(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_texture_save(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_texture_compile(sdk_project_t       *project,
                                sdk_project_asset_t *asset);

nu_status_t sdk_scene_load(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_scene_save(sdk_project_asset_t *asset, JSON_Object *jasset);
nu_status_t sdk_scene_compile(sdk_project_t       *project,
                              sdk_project_asset_t *asset);

nu_status_t sdk_json_parse_f32(const JSON_Object *object,
                               const nu_char_t   *name,
                               nu_f32_t          *v);
nu_status_t sdk_json_write_f32(JSON_Object     *object,
                               const nu_char_t *name,
                               nu_f32_t         value);
nu_status_t sdk_json_parse_u32(const JSON_Object *object,
                               const nu_char_t   *name,
                               nu_u32_t          *v);
nu_status_t sdk_json_write_u32(JSON_Object     *object,
                               const nu_char_t *name,
                               nu_u32_t         value);

nu_status_t cart_write(sdk_project_t *proj, const void *p, nu_size_t n);
nu_status_t cart_write_u32(sdk_project_t *proj, nu_u32_t v);
nu_status_t cart_write_f32(sdk_project_t *proj, nu_f32_t v);
nu_status_t cart_write_v2(sdk_project_t *proj, nu_v2_t v);
nu_status_t cart_write_v3(sdk_project_t *proj, nu_v3_t v);
nu_status_t cart_write_q4(sdk_project_t *proj, nu_q4_t v);
nu_status_t cart_write_m4(sdk_project_t *proj, nu_m4_t v);
nu_status_t cart_write_texture(sdk_project_t   *proj,
                               nu_u32_t         id,
                               nu_u32_t         size,
                               const nu_byte_t *data);

nu_status_t image_resize(nu_v2u_t         source_size,
                         const nu_byte_t *source_data,
                         nu_u32_t         target_size,
                         nu_byte_t       *target_data);

#endif
