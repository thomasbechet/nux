#ifndef RUNTIME_H
#define RUNTIME_H

#include <nulib.h>

typedef void (*runtime_log_callback_t)(nu_log_level_t   level,
                                       const nu_char_t *fmt,
                                       va_list          args);

typedef struct
{
    nu_sv_t                path;
    runtime_log_callback_t log_callback;
} runtime_info_t;

NU_API nu_status_t runtime_init(const runtime_info_t *info);

void *runtime_malloc(nu_size_t n);
void  runtime_free(void *p);

#endif
