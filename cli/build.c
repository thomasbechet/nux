#include "commands.h"

#include <vmcore/config.h>
#include <project/project.h>
#include <parson/parson.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_resize2.h>

static void
write_u32 (FILE *f, nu_u32_t v)
{
    nu_u32_t a = nu_u32_le(v);
    NU_ASSERT(fwrite(&a, sizeof(a), 1, f));
}
static void
write_v4u (FILE *f, nu_v4u_t v)
{
    for (nu_size_t i = 0; i < NU_V4_SIZE; ++i)
    {
        nu_u32_t a = nu_u32_le(v.data[i]);
        NU_ASSERT(fwrite(&a, sizeof(a), 1, f));
    }
}
static void
write_chunk_header (FILE *f, vm_chunk_header_t *header)
{
    // type / length
    write_u32(f, header->type);
    write_u32(f, header->length);
    // target
    switch (header->type)
    {
        case VM_CHUNK_RAW: {
        }
        break;
        case VM_CHUNK_WASM: {
        }
        break;
        case VM_CHUNK_TEXTURE: {
            write_u32(f, header->target.texture.slot);
            write_u32(f, header->target.texture.x);
            write_u32(f, header->target.texture.y);
            write_u32(f, header->target.texture.w);
            write_u32(f, header->target.texture.h);
        }
        break;
        case VM_CHUNK_MESH: {
        }
        break;
    }
}
void
cli_command_build (nu_sv_t path, nu_bool_t verbose)
{
    // Load package
    project_t project;
    project_load(&project, path);

    // Execute prebuild command
    if (nu_strlen(project.prebuild))
    {
        if (verbose)
        {
            printf("Execute prebuild command: %s\n", project.prebuild);
        }
#ifdef NU_PLATFORM_UNIX
        system(project.prebuild);
#endif
    }

    // Open cart
    FILE *f = fopen(project.target_path, "wb");
    if (!f)
    {
        printf("Failed to open file\n");
        goto cleanup0;
    }
    else
    {
        if (verbose)
        {
            printf("Cartridge %s generated.\n", project.target_path);
        }
    }

    // Write header
    const nu_u32_t version = 100;
    NU_ASSERT(fwrite(&version, sizeof(version), 1, f));
    const nu_u32_t chunk_count = project.entries_count;
    NU_ASSERT(fwrite(&chunk_count, sizeof(chunk_count), 1, f));

    // Compile entries
    for (nu_size_t i = 0; i < project.entries_count; ++i)
    {
        project_chunk_entry_t *entry = project.entries + i;
        switch (entry->header.type)
        {
            case VM_CHUNK_RAW:
                break;
            case VM_CHUNK_WASM: {
                nu_size_t  size;
                nu_byte_t *buffer;
                NU_ASSERT(nu_load_bytes(
                    nu_sv_cstr(entry->source_path), NU_NULL, &size));
                buffer = malloc(size);
                NU_ASSERT(buffer);
                NU_ASSERT(nu_load_bytes(
                    nu_sv_cstr(entry->source_path), buffer, &size));
                // header
                entry->header.length = size;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(buffer, size, 1, f));
                free(buffer);
            }
            break;
            case VM_CHUNK_TEXTURE: {
                int        w, h, n;
                nu_byte_t  fn[256];
                nu_byte_t *img = stbi_load(
                    (char *)entry->source_path, &w, &h, &n, STBI_default);
                NU_ASSERT(img);
                nu_v2u_t  target_size = nu_v2u(128, 128);
                nu_u32_t  target_comp = 4;
                nu_size_t length      = sizeof(nu_byte_t) * target_size.x
                                   * target_size.y * target_comp;
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

                // header
                entry->header.length = length;
                write_chunk_header(f, &entry->header);
                // data
                NU_ASSERT(fwrite(output, length, 1, f));

                free(output);
                stbi_image_free(img);
            }
            break;
            case VM_CHUNK_MESH: {
                // // header
                // write_chunk_header(f, entry, length);
                // // destination
                // write_u32(f, entry->dst.mesh.first);
                // // data
                // NU_ASSERT(fwrite(output, length, 1, f));
            }
            break;
        }
    }

    // Free resources
    fclose(f);
cleanup0:
    project_free(&project);
}
