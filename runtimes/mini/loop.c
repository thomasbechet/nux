#include "internal.h"

static struct
{
    nu_u32_t player;

    nu_v3_t  pos;
    nu_v3_t  vel;
    nu_v3_t  acc;
    nu_q4_t  rot;
    nu_f32_t yaw;
    nu_f32_t pitch;

    nu_f32_t fov;
    nu_f32_t speed;
} freecam;

void
init_debug_camera (nu_v3_t pos)
{
    freecam.player = 0;

    freecam.pos   = pos;
    freecam.vel   = NU_V3_ZEROS;
    freecam.acc   = NU_V3_ZEROS;
    freecam.rot   = nu_q4_identity();
    freecam.pitch = 0;
    freecam.yaw   = 0;

    freecam.fov   = 50;
    freecam.speed = 10;
}

nu_m4_t
debug_camera (nux_env_t env, nu_f32_t dt)
{
    nu_v3_t look = nu_v3(nux_axs(env, freecam.player, NUX_AXIS_RIGHTX) * 100,
                         nux_axs(env, freecam.player, NUX_AXIS_RIGHTY) * 100,
                         0);

    nu_v3_t move = nu_v3(nux_axs(env, freecam.player, NUX_AXIS_LEFTX),
                         0,
                         nux_axs(env, freecam.player, NUX_AXIS_LEFTY));
    move.y += nux_btn(env, freecam.player) & NUX_BUTTON_Y ? 1 : 0;
    move.y -= nux_btn(env, freecam.player) & NUX_BUTTON_B ? 1 : 0;
    move = nu_v3_normalize(move);

    // Translation
    nu_v3_t direction = NU_V3_ZEROS;

    direction = nu_v3_add(
        direction, nu_v3_muls(nu_q4_mulv3(freecam.rot, NU_V3_FORWARD), move.z));
    direction = nu_v3_add(
        direction, nu_v3_muls(nu_q4_mulv3(freecam.rot, NU_V3_RIGHT), move.x));
    direction = nu_v3_add(direction, nu_v3_muls(NU_V3_UP, move.y));
    direction = nu_v3_normalize(direction);

    // Rotation
    if (look.x != 0)
    {
        freecam.yaw += look.x * dt;
    }
    if (look.y != 0)
    {
        freecam.pitch -= look.y * dt;
    }
    freecam.pitch = NU_CLAMP(freecam.pitch, -90.0, 90.0);
    freecam.rot   = nu_q4_axis(NU_V3_UP, -nu_radian(freecam.yaw));
    freecam.rot   = nu_q4_mul(freecam.rot,
                            nu_q4_axis(NU_V3_RIGHT, -nu_radian(freecam.pitch)));

    // Compute sum of forces
    const nu_f32_t mass  = 10.0;
    nu_v3_t        force = NU_V3_ZEROS;

    // Apply movement
    if (nu_v3_norm(direction) > 0.001)
    {
        force = nu_v3_add(force, nu_v3_muls(direction, 5));
    }

    // Apply drag
    force = nu_v3_add(force, nu_v3_muls(freecam.vel, -0.5f));

    // Integrate
    freecam.pos     = nu_v3_add(freecam.pos,
                            nu_v3_add(nu_v3_muls(freecam.vel, dt),
                                      nu_v3_muls(freecam.acc, 0.5f * dt * dt)));
    nu_v3_t new_acc = nu_v3_muls(force, mass);
    nu_v3_t new_vel = nu_v3_add(
        freecam.vel, nu_v3_muls(nu_v3_add(freecam.acc, new_acc), 0.5f * dt));
    if (nu_v3_norm(new_vel) < 0.1)
    {
        new_vel = NU_V3_ZEROS;
    }
    freecam.acc = new_acc;
    freecam.vel = new_vel;

    nu_v3_t pos     = freecam.pos;
    nu_v3_t forward = nu_q4_mulv3(freecam.rot, NU_V3_FORWARD);
    nu_v3_t up      = nu_v3_normalize(nu_q4_mulv3(freecam.rot, NU_V3_UP));

    nu_m4_t view = nu_lookat(pos, nu_v3_add(pos, forward), up);
    nu_m4_t proj = nu_perspective(nu_radian(60.0),
                                  (nu_f32_t)NUX_SCREEN_WIDTH
                                      / (nu_f32_t)NUX_SCREEN_HEIGHT,
                                  0.5f,
                                  10000);

    return nu_m4_mul(proj, view);
}

// The '(x != 0 && y != 0)' test in the last line of this function
// may be omitted for a performance benefit if the radius of the
// circle is known to be non-zero.
void
plot4points (nux_env_t env, int cx, int cy, int x, int y, nux_u8_t c)
{
    nux_pset(env, cx + x, cy + y, c);
    if (x != 0)
    {
        nux_pset(env, cx - x, cy + y, c);
    }
    if (y != 0)
    {
        nux_pset(env, cx + x, cy - y, c);
    }
    if (x != 0 && y != 0)
    {
        nux_pset(env, cx - x, cy - y, c);
    }
}
void
plot8points (nux_env_t env, int cx, int cy, int x, int y, nux_u8_t c)
{
    plot4points(env, cx, cy, x, y, c);
    if (x != y)
    {
        plot4points(env, cx, cy, y, x, c);
    }
}
void
circle (nux_env_t env, int cx, int cy, int radius, nux_u8_t c)
{
    int error = -radius;
    int x     = radius;
    int y     = 0;

    // The following while loop may altered to 'while (x > y)' for a
    // performance benefit, as long as a call to 'plot4points' follows
    // the body of the loop. This allows for the elimination of the
    // '(x != y') test in 'plot8points', providing a further benefit.
    //
    // For the sake of clarity, this is not shown here.
    while (x >= y)
    {
        plot8points(env, cx, cy, x, y, c);
        error += y;
        ++y;
        error += y;
        // The following test may be implemented in assembly language in
        // most machines by testing the carry flag after adding 'y' to
        // the value of 'error' in the previous step, since 'error'
        // nominally has a negative value.
        if (error >= 0)
        {
            --x;
            error -= x;
            error -= x;
        }
    }
}
void
plot_circle (nux_env_t env, int xm, int ym, int r, nux_u8_t c)
{
    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do
    {
        nux_pset(env, xm - x, ym + y, c); /*   I. Quadrant */
        nux_pset(env, xm - y, ym - x, c); /*  II. Quadrant */
        nux_pset(env, xm + x, ym - y, c); /* III. Quadrant */
        nux_pset(env, xm + y, ym + x, c); /*  IV. Quadrant */
        r = err;
        if (r > x)
        {
            err += ++x * 2 + 1; /* e_xy+e_x > 0 */
        }
        if (r <= y)
        {
            err += ++y * 2 + 1; /* e_xy+e_y < 0 */
        }
    } while (x < 0);
}
void
raster_circle (nux_env_t env, int x0, int y0, int radius, nux_c8_t c)
{
    int f     = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x     = 0;
    int y     = radius;

    nux_pset(env, x0, y0 + radius, c);
    nux_pset(env, x0, y0 - radius, c);
    nux_pset(env, x0 + radius, y0, c);
    nux_pset(env, x0 - radius, y0, c);
    while (x < y)
    {
        // ddF_x == 2 * x + 1;
        // ddF_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        nux_pset(env, x0 + x, y0 + y, c);
        nux_pset(env, x0 - x, y0 + y, c);
        nux_pset(env, x0 + x, y0 - y, c);
        nux_pset(env, x0 - x, y0 - y, c);
        nux_pset(env, x0 + y, y0 + x, c);
        nux_pset(env, x0 - y, y0 + x, c);
        nux_pset(env, x0 + y, y0 - x, c);
        nux_pset(env, x0 - y, y0 - x, c);
    }
}

static nu_v4_t
vertex_shader (const nu_m4_t transform, nu_v3_t position)
{
    return nu_m4_mulv(transform, nu_v4_v3(position, 1));
}
static void
clip_edge_near (nu_v4_t  v0,
                nu_v4_t  v1,
                nu_v2_t  uv0,
                nu_v2_t  uv1,
                nu_v4_t *vclip,
                nu_v2_t *uvclip)
{
    const nu_v4_t near_plane = nu_v4(0, 0, 1, 1);
    nu_f32_t      d0         = nu_v4_dot(v0, near_plane);
    nu_f32_t      d1         = nu_v4_dot(v1, near_plane);
    nu_f32_t      s          = d0 / (d0 - d1);
    *vclip                   = nu_v4_lerp(v0, v1, s);
    *uvclip                  = nu_v2_lerp(uv0, uv1, s);
}
static nu_bool_t
clip_triangle (nu_v4_t   vertices[4],
               nu_v2_t   uvs[4],
               nu_u32_t  indices[6],
               nu_u32_t *indices_count)
{
    // Default triangle output
    *indices_count = 3;
    indices[0]     = 0;
    indices[1]     = 1;
    indices[2]     = 2;

    // Compute outsides
    nu_bool_t outside[3];
    for (nu_u32_t i = 0; i < 3; ++i)
    {
        outside[i] = (vertices[i].w <= 0) || (vertices[i].z < -vertices[i].w);
    }

    // Early test out
    if ((outside[0] & outside[1] & outside[2]) != 0)
    {
        return NU_FALSE;
    }

    // Early test in
    if ((outside[0] | outside[1] | outside[2]) == 0)
    {
        return NU_TRUE;
    }

    // Clip vertices
    for (nu_u32_t i = 0; i < 3; i++)
    {
        nu_v4_t  *vec, *vec_prev, *vec_next;
        nu_v2_t  *uv, *uv_prev, *uv_next;
        nu_bool_t out, out_prev, out_next;
        vec = &vertices[i];
        uv  = &uvs[i];
        out = outside[i];

        vec_next = &vertices[(i + 1) % 3];
        uv_next  = &uvs[(i + 1) % 3];
        out_next = outside[(i + 1) % 3];

        vec_prev = &vertices[(i + 2) % 3];
        uv_prev  = &uvs[(i + 2) % 3];
        out_prev = outside[(i + 2) % 3];

        if (out)
        {
            if (out_next) // 2 out case 1
            {
                clip_edge_near(*vec, *vec_prev, *uv, *uv_prev, vec, uv);
                // clip_edge_near(*vec, *vec_prev, vec);
            }
            else if (out_prev) // 2 out case 2
            {
                clip_edge_near(*vec, *vec_next, *uv, *uv_next, vec, uv);
                // clip_edge_near(*vec, *vec_next, vec);
            }
            else // 1 out
            {
                // Produce new vertex
                clip_edge_near(
                    *vec, *vec_next, *uv, *uv_next, &vertices[3], &uvs[3]);
                // clip_edge_near(*vec, *vec_next, &vertices[3]);
                *indices_count = 6;
                indices[3]     = i;
                indices[4]     = 3; // New vertex
                indices[5]     = (i + 1) % 3;

                // Clip existing vertex
                clip_edge_near(*vec, *vec_prev, *uv, *uv_prev, vec, uv);
                // clip_edge_near(*vec, *vec_prev, vec);

                return NU_TRUE;
            }
        }
    }

    return NU_TRUE;
}

static nu_v2i_t
pos_to_viewport (const nu_b2i_t vp, nu_v2_t v)
{
    // (p + 1) / 2
    v = nu_v2_adds(v, 1);
    v = nu_v2_muls(v, 0.5);

    // Convert to viewport
    v = nu_v2_mul(v, nu_v2_v2u(nu_b2i_size(vp)));
    v = nu_v2_add(v, nu_v2(vp.min.x, vp.min.y));

    v.y = NUX_SCREEN_HEIGHT - v.y;

    return nu_v2i(v.x, v.y);
}

static nu_i32_t
pixel_coverage (nu_v2i_t a, nu_v2i_t b, nu_v2i_t c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

static void
render_cube (nux_env_t env, nu_m4_t view_proj, nu_m4_t model)
{
    nu_m4_t mvp = nu_m4_mul(view_proj, model);

    const nu_b3_t box = nu_b3(nu_v3s(-.5), nu_v3s(.5));

    const nu_v3_t v0 = nu_v3(box.min.x, box.min.y, box.min.z);
    const nu_v3_t v1 = nu_v3(box.max.x, box.min.y, box.min.z);
    const nu_v3_t v2 = nu_v3(box.max.x, box.min.y, box.max.z);
    const nu_v3_t v3 = nu_v3(box.min.x, box.min.y, box.max.z);

    const nu_v3_t v4 = nu_v3(box.min.x, box.max.y, box.min.z);
    const nu_v3_t v5 = nu_v3(box.max.x, box.max.y, box.min.z);
    const nu_v3_t v6 = nu_v3(box.max.x, box.max.y, box.max.z);
    const nu_v3_t v7 = nu_v3(box.min.x, box.max.y, box.max.z);

    const nu_v3_t cube_positions[]
        = { v0, v1, v2, v2, v3, v0, v4, v6, v5, v6, v4, v7,
            v0, v3, v7, v7, v4, v0, v1, v5, v6, v6, v2, v1,
            v0, v4, v5, v5, v1, v0, v3, v2, v6, v6, v7, v3 };

    const nu_v2_t cube_uvs[] = {
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } }, { { 0, 0 } }, { { 1, 1 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 0, 0 } }, { { 0, 1 } }, { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } },
        { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 1, 0 } },
        { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } },
        { { 0, 1 } }, { { 1, 1 } }, { { 1, 1 } }, { { 1, 0 } }, { { 0, 0 } },
        { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
        { { 0, 0 } },
    };

    const nu_u8_t  cube_texture[4]   = { 1, 2, 2, 1 };
    const nu_v2u_t cube_texture_size = nu_v2u(2, 2);

    const nu_b2i_t vp = nu_b2i_xywh(0, 0, NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT);

    // Iterate over triangles
    for (nu_u32_t i = 0; i < NU_ARRAY_SIZE(cube_positions); i += 3)
    {
        // Apply vertex shader
        nu_v4_t vertices[4];
        nu_v2_t uvs[4];
        vertices[0] = vertex_shader(mvp, cube_positions[i + 0]);
        vertices[1] = vertex_shader(mvp, cube_positions[i + 1]);
        vertices[2] = vertex_shader(mvp, cube_positions[i + 2]);
        uvs[0]      = cube_uvs[i + 0];
        uvs[1]      = cube_uvs[i + 1];
        uvs[2]      = cube_uvs[i + 2];

        // Clip vertices
        nu_u32_t indices[6]    = { 0, 1, 2 };
        nu_u32_t indices_count = 3;
        if (!clip_triangle(vertices, uvs, indices, &indices_count))
        {
            continue;
        }

        // Perspective divide (NDC)
        nu_u32_t total_vertex = (indices_count > 3) ? 4 : 3;
        for (nu_u32_t i = 0; i < total_vertex; i++)
        {
            vertices[i] = nu_v4_muls(vertices[i], 1. / vertices[i].w);
        }

        // Iterate over clipped triangles
        for (nu_u32_t v = 0; v < indices_count; v += 3)
        {
            nu_v4_t v0  = vertices[indices[v + 0]];
            nu_v4_t v1  = vertices[indices[v + 1]];
            nu_v4_t v2  = vertices[indices[v + 2]];
            nu_v2_t uv0 = uvs[indices[v + 0]];
            nu_v2_t uv1 = uvs[indices[v + 1]];
            nu_v2_t uv2 = uvs[indices[v + 2]];

            nu_v2i_t v0vp = pos_to_viewport(vp, nu_v2(v0.x, v0.y));
            nu_v2i_t v1vp = pos_to_viewport(vp, nu_v2(v1.x, v1.y));
            nu_v2i_t v2vp = pos_to_viewport(vp, nu_v2(v2.x, v2.y));

            nu_f32_t area = pixel_coverage(v0vp, v1vp, v2vp);
            if (area <= 0)
            {
                continue;
            }

            nu_i32_t xmin = NU_MAX(0, NU_MIN(v0vp.x, NU_MIN(v1vp.x, v2vp.x)));
            nu_i32_t ymin = NU_MAX(0, NU_MIN(v0vp.y, NU_MIN(v1vp.y, v2vp.y)));
            nu_i32_t xmax = NU_MIN(NUX_SCREEN_WIDTH,
                                   NU_MAX(v0vp.x, NU_MAX(v1vp.x, v2vp.x)));
            nu_i32_t ymax = NU_MIN(NUX_SCREEN_HEIGHT,
                                   NU_MAX(v0vp.y, NU_MAX(v1vp.y, v2vp.y)));

            for (nu_i32_t y = ymin; y < ymax; ++y)
            {
                for (nu_i32_t x = xmin; x < xmax; x++)
                {
                    nu_v2i_t sample = nu_v2i(x, y);

                    // Compute weights
                    nu_f32_t w0 = pixel_coverage(v1vp, v2vp, sample);
                    nu_f32_t w1 = pixel_coverage(v2vp, v0vp, sample);
                    nu_f32_t w2 = pixel_coverage(v0vp, v1vp, sample);

                    nu_bool_t included = NU_TRUE;
                    // included &= (w0 == 0.0f) ? t0 : (w0 > 0.0f);
                    // included &= (w1 == 0.0f) ? t1 : (w1 > 0.0f);
                    // included &= (w2 == 0.0f) ? t2 : (w2 > 0.0f);
                    included = (w0 > 0 && w1 > 0 && w2 > 0);

                    const nu_f32_t area_inv = 1.0 / area;
                    w0 *= area_inv;
                    w1 *= area_inv;
                    w2 = 1.0 - w0 - w1;

                    // nu_f32_t a           = w0 * inv_vw0;
                    // nu_f32_t b           = w1 * inv_vw1;
                    // nu_f32_t c           = w2 * inv_vw2;
                    // nu_f32_t inv_sum_abc = 1.0f / (a + b + c);
                    // a *= inv_sum_abc;
                    // b *= inv_sum_abc;
                    // c *= inv_sum_abc;

                    if (included)
                    {
                        // Depth test
                        nu_f32_t depth = (w0 * v0.z + w1 * v1.z + w2 * v2.z);
                        if (depth < nux_zget(env, x, y))
                        {
                            nux_zset(env, x, y, depth);

                            // Texture sampling
                            nu_f32_t px = (w0 * uv0.x + w1 * uv1.x + w2 * uv2.x)
                                          * cube_texture_size.x;
                            nu_f32_t py = (w0 * uv0.y + w1 * uv1.y + w2 * uv2.y)
                                          * cube_texture_size.y;

                            nu_v2u_t texsize = cube_texture_size;

                            nu_u32_t uvx = NU_MIN(texsize.x - 1, (nu_u32_t)px);
                            nu_u32_t uvy = NU_MIN(texsize.y - 1,
                                                  texsize.y - 1 - (nu_u32_t)py);

                            nux_pset(
                                env, x, y, cube_texture[uvy * texsize.x + uvx]);
                        }
                    }
                }

                // nux_line(env, v0vp.x, v0vp.y, v1vp.x, v1vp.y, 3);
                // nux_line(env, v0vp.x, v0vp.y, v2vp.x, v2vp.y, 3);
                // nux_line(env, v1vp.x, v1vp.y, v2vp.x, v2vp.y, 3);
                // nux_pset(env, v0vp.x, v0vp.y, 4);
                // nux_pset(env, v1vp.x, v1vp.y, 4);
                // nux_pset(env, v2vp.x, v2vp.y, 4);
            }
        }
    }
}
static void
render_cubes (nux_env_t env, nu_m4_t view_proj)
{
    const nu_bool_t stresstest = NU_TRUE;
    for (nu_u32_t i = 0; i < (stresstest ? (25 * 25 * 25) : 25); ++i)
    {
        nu_u32_t x     = i % (stresstest ? 25 * 25 : 5);
        nu_u32_t y     = i / (stresstest ? 25 * 25 : 5);
        nu_m4_t  model = nu_m4_identity();
        model = nu_m4_mul(model, nu_m4_translate(nu_v3(x * 2, i % 10, y * 2)));
        model = nu_m4_mul(model, nu_m4_scale(nu_v3s(2)));
        model = nu_m4_mul(model, nu_m4_rotate_y(nu_radian(nux_time(env) * 0)));
        render_cube(env, view_proj, model);
    }
}

void
loop_init (nux_env_t env)
{
    nux_pal(env, 0, 0x0B3954);
    nux_pal(env, 1, 0xBFD7EA);
    nux_pal(env, 2, 0xFF6663);
    nux_pal(env, 3, 0xFF0000);
    nux_pal(env, 4, 0x00FF00);
    nux_pal(env, 5, 0x0000FF);
    nux_pal(env, 6, 0);
    nux_pal(env, 7, 0xFFFFFF);
    init_debug_camera(NU_V3_ZEROS);
}
void
loop_update (nux_env_t env)
{
    nux_cls(env, 6);
    nux_clsz(env);
    nu_v2i_t v0 = nu_v2i(50, 50);
    nu_v2i_t v1 = nu_v2i(100 + nu_cos(nux_time(env) * 0.5) * 50,
                         70 + nu_sin(nux_time(env) * 0.5) * 50);
    nu_v2i_t v2 = nu_v2i(80, 200);
    nux_filltri(env, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, 1);
    nux_line(env, 150, 150, 300, 20, 2);
    // circle(env, 100, 100, 50, 3);
    // plot_circle(env, 100, 100, 50, 3);

    nu_m4_t vp = debug_camera(env, nux_dt(env));
    render_cubes(env, vp);

    static nux_u32_t avg_fps = 0;
    static nu_f32_t  sum_fps = 0;
    sum_fps += nux_stat(env, NUX_STAT_FPS);
    if (nux_frame(env) % 10 == 0)
    {
        avg_fps = (nux_u32_t)(sum_fps / 10.);
        sum_fps = 0;
    }

    nux_cursor(env, 0, 0);
    nux_printfmt(env, 7, "FPS:%d", avg_fps);
    nux_printfmt(env, 7, "FRA:%d", nux_frame(env));
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
    nux_print(env, "TEST", 7);
}
