#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "api.h"

nu_status_t boot_load_cart(vm_t *vm, const nu_char_t *name);

#endif
