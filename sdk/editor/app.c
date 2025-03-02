#include <runtime.h>

static struct
{
    nu_char_t path[NU_PATH_MAX];
} app;

static void
init (void)
{
    runtime_init_instance(0, nu_sv_cstr(app.path));
}
static void
update (struct nk_context *ctx)
{
}

runtime_app_t
sdk_editor_app (nu_sv_t path)
{
    nu_sv_to_cstr(path, app.path, sizeof(app.path));
    return (runtime_app_t) { .init = init, .update = update };
}
