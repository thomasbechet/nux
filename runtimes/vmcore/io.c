#include "io.h"

#include "types.h"
#include "platform.h"
#include "wasm.h"

#define READ_U32(p)                                       \
    {                                                     \
        NU_ASSERT(os_read(&vm->user, (p), sizeof(*(p)))); \
        *(p) = nu_u32_le(*(p));                           \
    }

#define CART_HEADER_SIZE sizeof(nux_cart_header_t)

void
nux_cart_load_full (nux_vm_t *vm, const nu_byte_t *name)
{
    // Load cart header
    os_mount(vm->user, name);
    os_seek(vm->user, 0);
    READ_U32(&vm->cart.header.version);
    // TODO: validate
    READ_U32(&vm->cart.header.chunk_count);
    // TODO: validate

    // Load chunks
    nu_u32_t offset = CART_HEADER_SIZE;
    for (nu_size_t i = 0; i < vm->cart.header.chunk_count; ++i)
    {
        // read chunk header
        os_seek(vm->user, offset);
        nux_chunk_header_t header;
        READ_U32(&header.type);
        READ_U32(&header.length);
        READ_U32(&header.dst);

        switch (header.type)
        {
            case NUX_CHUNK_RAW:
                break;
            case NUX_CHUNK_WASM:
                nux_wasm_load(vm, &header);
                break;
            case NUX_CHUNK_TEXS:
                break;
            case NUX_CHUNK_TEXM:
                break;
            case NUX_CHUNK_TEXL:
                break;
            case NUX_CHUNK_MESH:
                break;
        }

        // next chunk
        offset += header.length;
    }
}
