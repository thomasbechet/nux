#include "internal.h"

int
main (int argc, char *argv[])
{
    config_t config = {
        .path  = argc > 1 ? argv[1] : NULL,
        .debug = true,
    };
    runtime_run(&config);
    return 0;
}
