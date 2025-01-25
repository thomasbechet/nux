#include "sdk.h"

#define STBIR_DEBUG

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
// #include <stb/stb_image_resize2.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

nu_byte_t *
sdk_load_image (nu_sv_t path, nu_v2u_t *size)
{
    nu_i32_t  w, h, n;
    nu_char_t fn[NU_PATH_MAX];
    nu_sv_to_cstr(path, fn, NU_PATH_MAX);
    nu_byte_t *img = stbi_load(fn, &w, &h, &n, STBI_rgb_alpha);
    NU_ASSERT(img);
    nu_v2u_t  target_size = nu_v2u(128, 128);
    nu_u32_t  target_comp = 4;
    nu_size_t length
        = sizeof(nu_byte_t) * target_size.x * target_size.y * target_comp;
    nu_byte_t *output = malloc(length);
    NU_ASSERT(output);
    NU_ASSERT(stbir_resize_uint8(
        img, w, h, 0, output, target_size.x, target_size.y, 0, target_comp));
    stbi_image_free(img);
    *size = target_size;
    return output;
}
