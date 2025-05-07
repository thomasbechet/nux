#include "internal.h"

int
main (int argc, char *argv[])
{
    runtime_config_t config = {
        .path  = NU_SV("."),
        .debug = NU_FALSE,
    };
    runtime_run(&config);
    return 0;
}
