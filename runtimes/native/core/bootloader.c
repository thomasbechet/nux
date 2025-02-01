#include "bootloader.h"

#include "vm.h"
#include "platform.h"

static NU_ENUM_MAP(cart_chunk_type_map,
                   NU_ENUM_NAME(CART_CHUNK_RAW, "raw"),
                   NU_ENUM_NAME(CART_CHUNK_MESH, "mesh"),
                   NU_ENUM_NAME(CART_CHUNK_WASM, "wasm"),
                   NU_ENUM_NAME(CART_CHUNK_TEXTURE, "texture"),
                   NU_ENUM_NAME(CART_CHUNK_MODEL, "model"));

nu_status_t
boot_init (vm_t *vm)
{
    vm->bootloader.heap = os_malloc(vm, BOOT_MEM_SIZE);
    NU_ASSERT(vm->bootloader.heap);
    return NU_SUCCESS;
}

nu_status_t
boot_load_cart (vm_t *vm, const nu_char_t *name)
{
    nu_status_t status = NU_SUCCESS;

    vm_log(vm, NU_LOG_INFO, "Loading cartridge %s", name);

    // Load cart header
    if (!os_cart_mount(vm, name))
    {
        vm_log(vm, NU_LOG_ERROR, "Failed to mount cartridge");
        return NU_FAILURE;
    }
    NU_CHECK(os_cart_seek(vm, 0), return NU_FAILURE);
    NU_CHECK(cart_read_u32(vm, &vm->bootloader.header.version),
             return NU_FAILURE);
    // TODO: validate

    // Load chunks
    os_cart_seek(vm, sizeof(cart_header_t));
    cart_chunk_header_t header;
    nu_u32_t            i = 0;
    while (cart_read_u32(vm, &header.type))
    {
        // read chunk header
        if (!cart_read_u32(vm, &header.length))
        {
            vm_log(vm, NU_LOG_ERROR, "Failed to read chunk header %d", i);
            return NU_FAILURE;
        }

        // read chunk
        switch (header.type)
        {
            case CART_CHUNK_RAW:
                break;
            case CART_CHUNK_WASM:
                status = cpu_load_wasm(vm, &header);
                break;
            case CART_CHUNK_TEXTURE:
                status = gpu_load_texture(vm, &header);
                break;
            case CART_CHUNK_MESH:
                status = gpu_load_mesh(vm, &header);
                break;
            case CART_CHUNK_MODEL:
                status = gpu_load_model(vm, &header);
                break;
        }
        if (!status)
        {
            vm_log(vm, NU_LOG_ERROR, "Failed to load chunk %d");
            return NU_FAILURE;
        }
        ++i;
    }

    vm_log(vm, NU_LOG_INFO, "Cartridge sucessfully loaded", name);

    return status;
}
