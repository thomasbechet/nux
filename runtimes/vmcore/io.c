#include "io.h"

#include "types.h"
#include "platform.h"
#include "vm.h"
#include "wasm.h"

static nu_u32_t
read_u32 (nux_vm_t *vm)
{
    nu_u32_t v;
    NU_ASSERT(os_read(&vm->user, &v, sizeof(v)));
    return nu_u32_le(v);
}
static nu_v4u_t
read_v4u (nux_vm_t *vm)
{
    nu_v4u_t v;
    v.x = read_u32(vm);
    v.y = read_u32(vm);
    v.z = read_u32(vm);
    v.w = read_u32(vm);
    return v;
}

void
nux_io_init (nux_vm_t *vm)
{
    vm->io.heap = vm_malloc(vm, NUX_IO_MEM_SIZE);
}
void
nux_cart_load_full (nux_vm_t *vm, const nu_byte_t *name)
{
    // Load cart header
    os_mount(vm->user, name);
    os_seek(vm->user, 0);
    vm->io.header.version = read_u32(vm);
    // TODO: validate
    vm->io.header.chunk_count = read_u32(vm);
    // TODO: validate

    // Load chunks
    os_seek(vm->user, sizeof(nux_cart_header_t));
    for (nu_size_t i = 0; i < vm->io.header.chunk_count; ++i)
    {
        // read chunk header
        nux_chunk_header_t header;
        header.type   = read_u32(vm);
        header.length = read_u32(vm);

        switch (header.type)
        {
            case NUX_CHUNK_RAW:
                break;
            case NUX_CHUNK_WASM:
                nux_wasm_load(vm, &header);
                break;
            case NUX_CHUNK_TEXTURE: {
                nu_u32_t        slot = read_u32(vm);
                nu_v4u_t        area = read_v4u(vm);
                const nu_size_t size = area.z * area.w * 4;
                NU_ASSERT(os_read(vm->user, vm->io.heap, size));
                os_write_texture(vm->user,
                                 slot,
                                 area.x,
                                 area.y,
                                 area.z,
                                 area.w,
                                 vm->io.heap);
            }
            break;
            case NUX_CHUNK_MESH: {
                nu_u32_t        first = read_u32(vm);
                nu_u32_t        count = read_u32(vm);
                const nu_size_t size
                    = count * NUX_VERTEX_SIZE * sizeof(nu_f32_t);
                NU_ASSERT(os_read(vm->user, vm->io.heap, size));
                os_write_vertex(vm->user, first, count, vm->io.heap);
            }
            break;
        }
    }
}
