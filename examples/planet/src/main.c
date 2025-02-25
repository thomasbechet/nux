#include "nux.h"
#define NU_IMPLEMENTATION
#include <nulib/nulib.h>
#define NUX_IMPLEMENTATION
#include <nuxlib/debug.h>
#include <nuxlib/freecam.h>

#define GOLDEN 1.618033988749
#define ICO_ID 1

static nu_pcg_t pcg;
static nu_f32_t rotation;

NU_VEC(nu_v3_t) positions;
NU_VEC(nu_v3_t) colors;

void
add_face (nu_v3_t v0, nu_v3_t v1, nu_v3_t v2)
{
    const nu_f32_t radius    = 50;
    v0                       = nu_v3_muls(nu_v3_normalize(v0), radius);
    v1                       = nu_v3_muls(nu_v3_normalize(v1), radius);
    v2                       = nu_v3_muls(nu_v3_normalize(v2), radius);
    *NU_VEC_PUSH(&positions) = v0;
    *NU_VEC_PUSH(&positions) = v1;
    *NU_VEC_PUSH(&positions) = v2;
    nu_u8_t    r             = nu_pcg_u32(&pcg) % 255;
    nu_u8_t    g             = nu_pcg_u32(&pcg) % 255;
    nu_u8_t    b             = nu_pcg_u32(&pcg) % 255;
    nu_color_t color         = nu_color(r, g, b, 128);
    *NU_VEC_PUSH(&colors)    = nu_color_to_vec3(color);
    *NU_VEC_PUSH(&colors)    = nu_color_to_vec3(color);
    *NU_VEC_PUSH(&colors)    = nu_color_to_vec3(color);
}

void
subdivide_at (nu_u32_t index)
{
    nu_v3_t v0  = positions.data[index + 0];
    nu_v3_t v1  = positions.data[index + 1];
    nu_v3_t v2  = positions.data[index + 2];
    nu_v3_t v01 = nu_v3_lerp(v0, v1, 0.5);
    nu_v3_t v12 = nu_v3_lerp(v1, v2, 0.5);
    nu_v3_t v20 = nu_v3_lerp(v2, v0, 0.5);
    v01         = nu_v3_normalize(v01);
    v12         = nu_v3_normalize(v12);
    v20         = nu_v3_normalize(v20);
    add_face(v01, v12, v20);
    add_face(v0, v01, v20);
    add_face(v01, v1, v12);
    add_face(v20, v12, v2);
}
void
subdivide (nu_u32_t lod, nu_u32_t *lodi, nu_u32_t *lods)
{
    nu_u32_t size = positions.size;
    for (nu_size_t i = lod; i < size; i += 3)
    {
        subdivide_at(i);
    }
    *lodi = size;
    *lods = positions.size - size;
}

void
start (void)
{
    pcg      = nu_pcg(0, 12948019270129571);
    rotation = 0;
    NU_VEC_INIT_A(&positions, malloc, 4096 * 128);
    NU_VEC_INIT_A(&colors, malloc, 4096 * 128);

    const float   c        = 0.525731112119134;
    const float   a        = c * GOLDEN;
    const nu_v3_t v[4 * 3] = {
        nu_v3(-c, +a, 0), nu_v3(+c, +a, 0), nu_v3(-c, -a, 0), nu_v3(+c, -a, 0),
        nu_v3(0, -c, +a), nu_v3(0, +c, +a), nu_v3(0, -c, -a), nu_v3(0, +c, -a),
        nu_v3(+a, 0, -c), nu_v3(+a, 0, +c), nu_v3(-a, 0, -c), nu_v3(-a, 0, +c),
    };
    const nu_u32_t idx[3 * 20]
        = { 0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
            4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
            6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1 };
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(idx); i += 3)
    {
        add_face(v[idx[i + 0]], v[idx[i + 1]], v[idx[i + 2]]);
    }

    nu_u32_t lod  = 0;
    nu_u32_t lods = positions.size;
    for (nu_u32_t i = 0; i < 6; ++i)
    {
        subdivide(lod, &lod, &lods);
    }

    init_mesh(
        ICO_ID, lods, PRIMITIVE_TRIANGLES, VERTEX_POSITION | VERTEX_COLOR);
    update_mesh(ICO_ID, VERTEX_POSITION, 0, lods, positions.data + lod);
    update_mesh(ICO_ID, VERTEX_COLOR, 0, lods, colors.data + lod);
    init_model(2, 1);
    nu_m4_t identity = nu_m4_identity();
    update_model(2, 0, ICO_ID, 0, -1, identity.data);

    nux_init_debug_camera(nu_v3(40, 0, 0));
}

void
update (void)
{
    nu_m4_t lookat = nu_lookat(nu_v3(5, 5, 5), NU_V3_ZEROS, NU_V3_UP);
    set_view(lookat.data);
    nu_m4_t projection = nu_perspective(
        nu_radian(30), (nu_f32_t)SCREEN_WIDTH / SCREEN_HEIGHT, 0.5, 100);
    set_projection(projection.data);

    // rotation += delta_time() * 10;

    nux_debug_camera(delta_time(), NU_NULL);

    nu_m4_t trans = nu_m4_identity();
    trans         = nu_m4_mul(trans, nu_m4_rotate_y(nu_radian(rotation)));
    set_transform(trans.data);
    draw_model(2);
    draw_volume();

    nux_debug();
}
