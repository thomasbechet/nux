#include "vmcore.h"

#include "wasm.h"

void
nu_vmcore_init (const nu_byte_t *buffer, nu_size_t size)
{
    nu_wasm_init(buffer, size);
}
void
nu_vmcore_free (void)
{
}
void
nu_vmcore_update (void)
{
}
