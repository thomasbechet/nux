#include "cartridge.h"

#include "vm.h"
#include "platform.h"

nu_size_t
cart_read (vm_t *vm, void *p, nu_size_t n)
{
    return os_cart_read(vm, p, n);
}
nu_status_t
cart_read_u32 (vm_t *vm, nu_u32_t *v)
{
    NU_CHECK(os_cart_read(vm, v, sizeof(*v)) == sizeof(*v), return NU_FAILURE);
    *v = nu_u32_le(*v);
    return NU_SUCCESS;
}
nu_status_t
cart_read_f32 (vm_t *vm, nu_f32_t *v)
{
    NU_CHECK(os_cart_read(vm, v, sizeof(*v)) == sizeof(*v), return NU_FAILURE);
    *v = nu_f32_le(*v);
    return NU_SUCCESS;
}
nu_status_t
cart_read_m4 (vm_t *vm, nu_m4_t *v)
{
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        NU_CHECK(cart_read_f32(vm, &v->data[i]), return NU_FAILURE);
    }
    return NU_SUCCESS;
}
