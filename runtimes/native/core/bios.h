#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "syscall.h"

nu_status_t bios_load_cart(vm_t *vm, const nu_char_t *name);

#endif
