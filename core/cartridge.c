#include "internal.h"

// static nux_status_t
// cart_read (nux_env_t env, void *p, nux_u32_t n)
// {
//     if (nux_platform_read(env->inst, p, n) != n)
//     {
//         nux_set_error(env, NUX_ERROR_CART_EOF);
//         return NUX_FAILURE;
//     }
//     return NUX_SUCCESS;
// }
// static nux_status_t
// cart_read_u32 (nux_env_t env, nu_u32_t *v)
// {
//     NU_CHECK(cart_read(env, v, sizeof(*v)), return NUX_FAILURE);
//     *v = nu_u32_le(*v);
//     return NUX_SUCCESS;
// }
// static nux_status_t
// cart_read_f32 (nux_env_t env, nu_f32_t *v)
// {
//     NU_CHECK(cart_read(env, v, sizeof(*v)), return NUX_FAILURE);
//     *v = nu_f32_le(*v);
//     return NUX_SUCCESS;
// }
// nux_status_t
// cart_read_v3 (nux_env_t env, nu_v3_t *v)
// {
//     for (nux_u32_t i = 0; i < NU_V3_SIZE; ++i)
//     {
//         NU_CHECK(cart_read_f32(env, &v->data[i]), return NUX_FAILURE);
//     }
//     return NUX_SUCCESS;
// }
// nux_status_t
// cart_read_q4 (nux_env_t env, nu_q4_t *v)
// {
//     for (nux_u32_t i = 0; i < NU_Q4_SIZE; ++i)
//     {
//         NU_CHECK(cart_read_f32(env, &v->data[i]), return NUX_FAILURE);
//     }
//     return NUX_SUCCESS;
// }
// nux_status_t
// cart_read_m4 (nux_env_t env, nu_m4_t *v)
// {
//     for (nux_u32_t i = 0; i < NU_M4_SIZE; ++i)
//     {
//         NU_CHECK(cart_read_f32(env, &v->data[i]), return NUX_FAILURE);
//     }
//     return NUX_SUCCESS;
// }
//
// nux_status_t
// nux_cart_parse_header (const void *data, nux_cart_header_t *header)
// {
//     const nu_byte_t *ptr = data;
//     header->version      = nu_u32_le(*((nu_u32_t *)ptr));
//     ptr += sizeof(nu_u32_t);
//     header->entry_count = nu_u32_le(*((nu_u32_t *)ptr));
//     // TODO: validate
//     return NUX_SUCCESS;
// }
