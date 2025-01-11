#include "importer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_resize2.h>

nu_byte_t *
importer_load_image (nu_sv_t path, nu_v2u_t *size)
{
    nu_i32_t  w, h, n;
    nu_char_t fn[NU_PATH_MAX];
    nu_sv_to_cstr(path, fn, NU_PATH_MAX);
    nu_byte_t *img = stbi_load(fn, &w, &h, &n, STBI_default);
    NU_ASSERT(img);
    nu_v2u_t  target_size = nu_v2u(128, 128);
    nu_u32_t  target_comp = 4;
    nu_size_t length
        = sizeof(nu_byte_t) * target_size.x * target_size.y * target_comp;
    nu_byte_t *output = malloc(length);
    NU_ASSERT(output);
    NU_ASSERT(stbir_resize_uint8_linear(img,
                                        w,
                                        h,
                                        w * n,
                                        output,
                                        target_size.x,
                                        target_size.y,
                                        target_size.x * target_comp,
                                        STBIR_RGBA));
    stbi_image_free(img);
    *size = target_size;
    return output;
}
