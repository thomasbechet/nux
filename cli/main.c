#include <native/runtime.h>

#include <argparse/argparse.h>

static const char *const usages[] = {
    "basic [options] [[--] args]",
    "basic [options]",
    NULL,
};

#define PERM_READ  (1 << 0)
#define PERM_WRITE (1 << 1)
#define PERM_EXEC  (1 << 2)

int
main (int argc, const char *argv[])
{
    const char            *cart_path = NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_STRING('c', "path", &cart_path, "path of the cart", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(
        &argparse,
        "\nA brief description of what the program does and how it works.",
        "\nAdditional description of the program after the description of the "
        "arguments.");
    argc = argparse_parse(&argparse, argc, argv);
    if (cart_path != NULL)
    {
        printf("path: %s\n", cart_path);
    }
    nux_runtime_run(argc, argv);
    return 0;
}
