#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "cartridge.h"

#define BOOT_MEM_SIZE NU_MEM_1M

typedef struct
{
    cart_header_t header;
    void         *heap;
} bootloader_t;

nu_status_t boot_init(vm_t *vm);
nu_status_t boot_load_cart(vm_t *vm, const nu_char_t *name);

#endif
