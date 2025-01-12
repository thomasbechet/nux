#ifndef IMPORTER_ERROR_H
#define IMPORTER_ERROR_H

#include <nulib.h>

typedef enum
{
    IMPORTER_ERROR_TEST
} importer_error_code_t;

typedef struct
{
    importer_error_code_t code;
} importer_error_t;

#endif
