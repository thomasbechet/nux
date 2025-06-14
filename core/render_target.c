#include "internal.h"

nux_u32_t
nux_render_target_new (nux_env_t *env, nux_u32_t w, nux_u32_t h)
{
    nux_frame_t frame = nux_begin_frame(env);

    // Create object
    nux_u32_t            id;
    nux_render_target_t *rt
        = NUX_NEW(env, NUX_OBJECT_RENDER_TARGET, nux_render_target_t, &id);
    NUX_CHECKM(rt, "Failed to create render target", return NUX_NULL);

    // Create texture
    rt->texture = nux_texture_new(env, NUX_TEXTURE_FORMAT_RGBA, w, h);
    NUX_CHECKM(
        rt->texture, "Failed to create texture render target", goto cleanup);

    // Create framebuffer
    nux_u32_t *slot = nux_u32_vec_pop(&env->inst->free_framebuffer_slots);
    NUX_CHECKM(slot, "Out of gpu framebuffer slots", goto cleanup);
    rt->slot = *slot;

    nux_texture_t *tex = nux_get(env, NUX_OBJECT_TEXTURE, rt->texture);
    NUX_CHECKM(
        nux_os_create_framebuffer(env->inst->userdata, rt->slot, tex->slot),
        "Failed to create framebuffer",
        goto cleanup);

    return id;

cleanup:
    nux_reset_frame(env, frame);
    return NUX_NULL;
}
void
nux_render_target_cleanup (nux_env_t *env, void *data)
{
    nux_render_target_t *rt = data;
    if (rt->slot)
    {
        nux_u32_vec_pushv(&env->inst->free_framebuffer_slots, rt->slot);
    }
}
