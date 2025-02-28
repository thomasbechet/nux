#include "runtime.h"
#include "core/platform.h"

void
os_inspect (vm_t *vm, const nu_char_t *name, sys_inspect_type_t type, void *p)
{
    switch (type)
    {
        case SYS_INSPECT_I32:
            logger_log(NU_LOG_INFO, "%lf", *(const nu_f32_t *)p);
            break;
        case SYS_INSPECT_F32:
            logger_log(NU_LOG_INFO, "%d", *(const nu_i32_t *)p);
            break;
    }
}
