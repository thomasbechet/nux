#ifndef WAMR_H
#define WAMR_H

#include "core/vm.h"

nu_status_t wamr_init(nu_bool_t debug);
void        wamr_free(void);

#endif
