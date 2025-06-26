#include "internal.h"

int
main (int argc, char *argv[])
{
    config_t config = {
        .path  = ".",
        .debug = true,
    };
    runtime_run(&config);
    return 0;
}
