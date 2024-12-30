#include "builder.h"

#include <stdio.h>
#include <constants.h>

static nu_byte_t *
load_bytes (const nu_byte_t *filename, nu_size_t *size)
{
    FILE *f = fopen((char *)filename, "rb");
    if (!f)
    {
        printf("Failed to open file %s\n", filename);
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)malloc(fsize);
    fread(bytes, fsize, 1, f);
    fclose(f);
    *size = fsize;
    return bytes;
}

void
nux_build_cart (const nu_byte_t *path)
{
    nu_size_t  size;
    nu_byte_t *buffer = load_bytes(path, &size);
    NU_ASSERT(buffer);

    FILE *f = fopen("cart.bin", "wb");
    if (!f)
    {
        printf("Failed to open file\n");
        return;
    }

    // version
    const nu_u32_t version = 100;
    NU_ASSERT(fwrite(&version, sizeof(version), 1, f));

    // chunk count
    const nu_u32_t chunk_count = 1;
    NU_ASSERT(fwrite(&chunk_count, sizeof(chunk_count), 1, f));

    // type
    nu_u32_t wasm_type = NUX_CHUNK_WASM;
    NU_ASSERT(fwrite(&wasm_type, sizeof(wasm_type), 1, f));

    // length
    nu_u32_t wasm_length = size;
    NU_ASSERT(fwrite(&wasm_length, sizeof(wasm_length), 1, f));

    // dst
    nu_u32_t wasm_dst = 0;
    NU_ASSERT(fwrite(&wasm_dst, sizeof(wasm_dst), 1, f));

    // wasm data
    nu_size_t n;
    NU_ASSERT(fwrite(buffer, size, 1, f));

    fclose(f);
}
