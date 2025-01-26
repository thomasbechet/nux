#include "iou.h"

#include "cpu.h"
#include "gpu.h"
#include "platform.h"

static NU_ENUM_MAP(cart_chunk_type_map,
                   NU_ENUM_NAME(CART_CHUNK_RAW, "raw"),
                   NU_ENUM_NAME(CART_CHUNK_MESH, "mesh"),
                   NU_ENUM_NAME(CART_CHUNK_WASM, "wasm"),
                   NU_ENUM_NAME(CART_CHUNK_TEXTURE, "texture"),
                   NU_ENUM_NAME(CART_CHUNK_MODEL, "model"));

nu_status_t
iou_init (vm_t *vm)
{
    vm->iou.heap = os_malloc(vm, IOU_MEM_SIZE);
    NU_ASSERT(vm->iou.heap);
    return NU_SUCCESS;
}
nu_status_t
iou_load_cart (vm_t *vm, const nu_char_t *name)
{
    nu_status_t status = NU_SUCCESS;

    iou_log(vm, NU_LOG_INFO, "Loading cartridge %s", name);

    // Load cart header
    if (!os_iou_mount(vm, name))
    {
        iou_log(vm, NU_LOG_ERROR, "Failed to mount cartridge");
        return NU_FAILURE;
    }
    NU_CHECK(os_iou_seek(vm, 0), return NU_FAILURE);
    NU_CHECK(iou_read_u32(vm, &vm->iou.header.version), return NU_FAILURE);
    // TODO: validate
    NU_CHECK(iou_read_u32(vm, &vm->iou.header.chunk_count), return NU_FAILURE);
    // TODO: validate

    // Load chunks
    os_iou_seek(vm, sizeof(cart_header_t));
    for (nu_size_t i = 0; i < vm->iou.header.chunk_count; ++i)
    {
        // read chunk header
        cart_chunk_header_t header;
        status &= iou_read_u32(vm, &header.type);
        status &= iou_read_u32(vm, &header.length);
        if (!status)
        {
            iou_log(vm, NU_LOG_ERROR, "Failed to read chunk header %d", i);
            return NU_FAILURE;
        }

        iou_log(vm,
                NU_LOG_INFO,
                "[%d] type: '%s', length: %d bytes",
                i,
                nu_enum_to_cstr(header.type, cart_chunk_type_map),
                header.length);

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
            iou_log(vm, NU_LOG_ERROR, "Failed to load chunk %d");
            return NU_FAILURE;
        }
    }

    iou_log(vm, NU_LOG_INFO, "Cartridge sucessfully loaded", name);

    return status;
}
nu_size_t
iou_read (vm_t *vm, void *p, nu_size_t n)
{
    return os_iou_read(vm, p, n);
}
nu_status_t
iou_read_u32 (vm_t *vm, nu_u32_t *v)
{
    NU_CHECK(os_iou_read(vm, v, sizeof(*v)) == sizeof(*v), return NU_FAILURE);
    *v = nu_u32_le(*v);
    return NU_SUCCESS;
}
nu_status_t
iou_read_f32 (vm_t *vm, nu_f32_t *v)
{
    NU_CHECK(os_iou_read(vm, v, sizeof(*v)) == sizeof(*v), return NU_FAILURE);
    *v = nu_f32_le(*v);
    return NU_SUCCESS;
}
nu_status_t
iou_read_m4 (vm_t *vm, nu_m4_t *v)
{
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        NU_CHECK(iou_read_f32(vm, &v->data[i]), return NU_FAILURE);
    }
    return NU_SUCCESS;
}

void
iou_log (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    iou_vlog(vm, level, fmt, args);
    va_end(args);
}
void
iou_vlog (vm_t *vm, nu_log_level_t level, const nu_char_t *fmt, va_list args)
{
    os_iou_vlog(vm, level, fmt, args);
}
