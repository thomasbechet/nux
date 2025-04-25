#include "internal.h"

void
nux_rectfill (nux_env_t env,
              nux_i32_t x0,
              nux_i32_t y0,
              nux_i32_t x1,
              nux_i32_t y1,
              nux_u8_t  color)
{
    nux_i32_t minx = NU_MIN(x0, x1);
    nux_i32_t miny = NU_MIN(y0, y1);
    nux_i32_t maxx = NU_MAX(x0, x1);
    nux_i32_t maxy = NU_MAX(y0, y1);
    for (nux_i32_t y = miny; y <= maxy; ++y)
    {
        for (nux_i32_t x = minx; x <= maxx; ++x)
        {
            nux_pset(env, x, y, color);
        }
    }
}
void
nux_pset (nux_env_t env, nux_i32_t x, nux_i32_t y, nux_u8_t color)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return);
    env->inst->memory[NUX_RAM_SCREEN + y * NUX_SCREEN_WIDTH + x]
        = nux_palc(env, color);
}
nux_u8_t
nux_pget (nux_env_t env, nux_i32_t x, nux_i32_t y)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return 0);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return 0);
    return env->inst->memory[NUX_RAM_SCREEN + y * NUX_SCREEN_WIDTH + x];
}
void
nux_zset (nux_env_t env, nux_i32_t x, nux_i32_t y, nux_f32_t depth)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return);
    nux_f32_t *z = NUX_MEMPTR(env->inst, NUX_RAM_ZBUFFER, nux_f32_t);
    z[y * NUX_SCREEN_WIDTH + x] = depth;
}
nux_f32_t
nux_zget (nux_env_t env, nux_i32_t x, nux_i32_t y)
{
    NU_CHECK(x >= 0 && x < NUX_SCREEN_WIDTH, return 0);
    NU_CHECK(y >= 0 && y < NUX_SCREEN_HEIGHT, return 0);
    const nux_f32_t *z
        = NUX_MEMPTR(env->inst, NUX_RAM_ZBUFFER, const nux_f32_t);
    return z[y * NUX_SCREEN_WIDTH + x];
}
void
nux_line (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u8_t  c)
{
    nu_i32_t dx  = NU_ABS(x1 - x0);
    nu_i32_t sx  = x0 < x1 ? 1 : -1;
    nu_i32_t dy  = -NU_ABS(y1 - y0);
    nu_i32_t sy  = y0 < y1 ? 1 : -1;
    nu_i32_t err = dx + dy;
    nu_i32_t e2;
    for (;;)
    {
        nux_pset(env, x0, y0, c);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void
nux_trifill (nux_env_t env,
             nux_i32_t x0,
             nux_i32_t y0,
             nux_i32_t x1,
             nux_i32_t y1,
             nux_i32_t x2,
             nux_i32_t y2,
             nux_u8_t  c)
{
    // v0.y <= v1.y <= v2.y
    nu_v2i_t v0 = nu_v2i(x0, y0);
    nu_v2i_t v1 = nu_v2i(x1, y1);
    nu_v2i_t v2 = nu_v2i(x2, y2);
    if (y0 == y1 && y0 == y2)
    {
        nux_line(env,
                 NU_MIN(x0, NU_MIN(x1, x2)),
                 y0,
                 NU_MAX(x0, NU_MAX(x1, x2)),
                 y0,
                 c);
        return;
    }
    if (v0.y > v1.y)
    {
        NU_SWAP(v0, v1, nu_v2i_t);
    }
    if (v0.y > v2.y)
    {
        NU_SWAP(v0, v2, nu_v2i_t);
    }
    if (v1.y > v2.y)
    {
        NU_SWAP(v1, v2, nu_v2i_t);
    }

    // 3 bresenham iterations :
    //  v0 -> v1 (bresenham0) (check transition to v2)
    //  v1 -> v2 (bresenham1)
    //  v0 -> v2 (bresenham1) (longest iteration)

    nu_i32_t  dx0  = NU_ABS(v1.x - v0.x);
    nu_i32_t  dx1  = NU_ABS(v2.x - v0.x);
    nu_i32_t  sx0  = v0.x < v1.x ? 1 : -1;
    nu_i32_t  sx1  = v0.x < v2.x ? 1 : -1;
    nu_i32_t  dy0  = -NU_ABS(v1.y - v0.y);
    nu_i32_t  dy1  = -NU_ABS(v2.y - v0.y);
    nu_i32_t  sy0  = v0.y < v1.y ? 1 : -1;
    nu_i32_t  sy1  = v0.y < v2.y ? 1 : -1;
    nu_i32_t  err0 = dx0 + dy0;
    nu_i32_t  err1 = dx1 + dy1;
    nu_i32_t  e20, e21;
    nu_i32_t  curx0 = v0.x;
    nu_i32_t  curx1 = v0.x;
    nu_i32_t  cury0 = v0.y;
    nu_i32_t  cury1 = v0.y;
    nu_bool_t lower = NU_FALSE;

bresenham0:
    for (;;)
    {
        if (curx0 == v1.x && cury0 == v1.y && !lower)
        {
            // End of first iteration, transition to v1 -> v2
            dx0   = NU_ABS(v2.x - v1.x);
            sx0   = v1.x < v2.x ? 1 : -1;
            dy0   = -NU_ABS(v2.y - v1.y);
            sy0   = v1.y < v2.y ? 1 : -1;
            err0  = dx0 + dy0;
            curx0 = v1.x;
            cury0 = v1.y;
            lower = NU_TRUE;
        }
        e20 = 2 * err0;
        if (e20 >= dy0)
        {
            err0 += dy0;
            curx0 += sx0;
        }
        if (e20 <= dx0)
        {
            err0 += dx0;
            cury0 += sy0;
            goto bresenham1;
        }
    }

bresenham1:
    for (;;)
    {
        e21 = 2 * err1;
        if (e21 >= dy1)
        {
            err1 += dy1;
            curx1 += sx1;
        }
        if (e21 <= dx1)
        {
            err1 += dx1;
            cury1 += sy1;
            goto processline;
        }
    }

processline:
    for (nu_i32_t x = NU_MIN(curx0, curx1); x < NU_MAX(curx0, curx1); ++x)
    {
        nux_pset(env, x, cury1, c);
    }
    if (cury1 == v2.y)
    {
        return;
    }
    goto bresenham0;
}
void
nux_circ (nux_env_t env, nu_i32_t xm, nu_i32_t ym, nu_i32_t r, nux_u8_t c)
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
nux_rect (nux_env_t env,
          nux_i32_t x0,
          nux_i32_t y0,
          nux_i32_t x1,
          nux_i32_t y1,
          nux_u8_t  c)
{
    nux_i32_t xmin = NU_MIN(x0, x1);
    nux_i32_t xmax = NU_MAX(x0, x1);
    nux_i32_t ymin = NU_MIN(y0, y1);
    nux_i32_t ymax = NU_MAX(y0, y1);
    for (nux_i32_t x = xmin; x <= xmax; ++x)
    {
        nux_pset(env, x, y0, c);
        nux_pset(env, x, y1, c);
    }
    for (nux_i32_t y = ymin; y <= ymax; ++y)
    {
        nux_pset(env, x0, y, c);
        nux_pset(env, x1, y, c);
    }
}

static inline nu_v4_t
vertex_shader (const nu_m4_t transform, nu_f32_t x, nu_f32_t y, nu_f32_t z)
{
    nu_v4_t ret;
    ret.x
        = transform.x1 * x + transform.y1 * y + transform.z1 * z + transform.w1;
    ret.y
        = transform.x2 * x + transform.y2 * y + transform.z2 * z + transform.w2;
    ret.z
        = transform.x3 * x + transform.y3 * y + transform.z3 * z + transform.w3;
    ret.w
        = transform.x4 * x + transform.y4 * y + transform.z4 * z + transform.w4;
    return ret;
}
static inline void
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
static nu_u32_t
clip_triangle (nu_v4_t vertices[4], nu_v2_t uvs[4], nu_u32_t indices[6])
{
    // Default triangle output
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    // Compute outsides
    nu_bool_t outside[3];
    for (nu_u32_t i = 0; i < 3; ++i)
    {
        outside[i] = (vertices[i].w <= 0) || (vertices[i].z < -vertices[i].w);
    }

    // Early test out
    if ((outside[0] & outside[1] & outside[2]) != 0)
    {
        return 0;
    }

    // Early test in
    if ((outside[0] | outside[1] | outside[2]) == 0)
    {
        return 3;
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
            }
            else if (out_prev) // 2 out case 2
            {
                clip_edge_near(*vec, *vec_next, *uv, *uv_next, vec, uv);
            }
            else // 1 out
            {
                // Produce new vertex
                clip_edge_near(
                    *vec, *vec_next, *uv, *uv_next, &vertices[3], &uvs[3]);
                indices[3] = i;
                indices[4] = 3; // New vertex
                indices[5] = (i + 1) % 3;

                // Clip existing vertex
                clip_edge_near(*vec, *vec_prev, *uv, *uv_prev, vec, uv);

                return 6;
            }
        }
    }

    return 3;
}

static inline nu_v2i_t
pos_to_viewport (nu_b2i_t vp, nu_f32_t x, nu_f32_t y)
{
    nu_v2u_t size = nu_b2i_size(vp);
    nu_i32_t px   = (x + 1) * 0.5 * (nu_f32_t)size.x;
    nu_i32_t py   = size.y - (y + 1) * 0.5 * (nu_f32_t)size.y;
    return nu_v2i(vp.min.x + px, vp.min.y + py);
}

static inline nu_i32_t
pixel_coverage (nu_v2i_t a, nu_v2i_t b, nu_i32_t x, nu_i32_t y)
{
    return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
}

void
nux_mesht (nux_env_t        env,
           const nux_f32_t *positions,
           const nux_f32_t *uvs,
           nux_u32_t        count,
           const nux_u8_t  *tex,
           nux_u32_t        texw,
           nux_u32_t        texh,
           const nux_f32_t *m)
{
    const nu_f32_t *cameye
        = NUX_MEMPTR(env->inst, NUX_RAM_CAM_EYE, const nu_f32_t);
    const nu_f32_t *camcenter
        = NUX_MEMPTR(env->inst, NUX_RAM_CAM_CENTER, const nu_f32_t);
    const nu_f32_t *camup
        = NUX_MEMPTR(env->inst, NUX_RAM_CAM_UP, const nu_f32_t);
    nu_f32_t        fov = NUX_MEMGET(env->inst, NUX_RAM_CAM_FOV, nu_f32_t);
    const nu_u32_t *viewport
        = NUX_MEMPTR(env->inst, NUX_RAM_CAM_VIEWPORT, const nu_u32_t);

    nu_m4_t view = nu_lookat(nu_v3(cameye[0], cameye[1], cameye[2]),
                             nu_v3(camcenter[0], camcenter[1], camcenter[2]),
                             nu_v3(camup[0], camup[1], camup[2]));
    nu_m4_t proj = nu_perspective(
        nu_radian(fov), (nu_f32_t)viewport[2] / viewport[3], 1, 500);
    nu_m4_t  view_proj = nu_m4_mul(proj, view);
    nu_m4_t  model     = nu_m4(m);
    nu_m4_t  mvp       = nu_m4_mul(view_proj, model);
    nu_b2i_t vp
        = nu_b2i_xywh(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Iterate over triangles
    for (nu_u32_t i = 0; i < count; i += 3)
    {
        // Apply vertex shader
        nu_v4_t vertex_positions[4];
        nu_v2_t vertex_uvs[4];
        for (nu_u32_t j = 0; j < 3; ++j)
        {
            vertex_positions[j] = vertex_shader(mvp,
                                                positions[(i + j) * 3 + 0],
                                                positions[(i + j) * 3 + 1],
                                                positions[(i + j) * 3 + 2]);
            vertex_uvs[j].x     = uvs[(i + j) * 2 + 0];
            vertex_uvs[j].y     = uvs[(i + j) * 2 + 1];
        }

        // Clip vertices
        nu_u32_t indices[6];
        nu_u32_t indices_count
            = clip_triangle(vertex_positions, vertex_uvs, indices);
        if (!indices_count)
        {
            continue;
        }

        // Perspective divide (NDC)
        for (nu_u32_t i = 0; i < 4; i++)
        {
            vertex_positions[i].x /= vertex_positions[i].w;
            vertex_positions[i].y /= vertex_positions[i].w;
            vertex_positions[i].z /= vertex_positions[i].w;
        }

        // Iterate over clipped triangles
        for (nu_u32_t v = 0; v < indices_count; v += 3)
        {
            nu_v4_t v0 = vertex_positions[indices[v + 0]];
            nu_v4_t v1 = vertex_positions[indices[v + 1]];
            nu_v4_t v2 = vertex_positions[indices[v + 2]];

            nu_v2i_t v0vp = pos_to_viewport(vp, v0.x, v0.y);
            nu_v2i_t v1vp = pos_to_viewport(vp, v1.x, v1.y);
            nu_v2i_t v2vp = pos_to_viewport(vp, v2.x, v2.y);

            nu_i32_t area = pixel_coverage(v0vp, v1vp, v2vp.x, v2vp.y);
            if (area <= 0)
            {
                continue;
            }

            nu_v2_t uv0 = vertex_uvs[indices[v + 0]];
            nu_v2_t uv1 = vertex_uvs[indices[v + 1]];
            nu_v2_t uv2 = vertex_uvs[indices[v + 2]];

            // Keep inv weights for perspective correction
            nu_f32_t inv_vw0 = 1. / v0.w;
            nu_f32_t inv_vw1 = 1. / v1.w;
            nu_f32_t inv_vw2 = 1. / v2.w;

            nu_i32_t xmin
                = NU_MAX(vp.min.x, NU_MIN(v0vp.x, NU_MIN(v1vp.x, v2vp.x)));
            nu_i32_t ymin
                = NU_MAX(vp.min.y, NU_MIN(v0vp.y, NU_MIN(v1vp.y, v2vp.y)));
            nu_i32_t xmax
                = NU_MIN(vp.max.x, NU_MAX(v0vp.x, NU_MAX(v1vp.x, v2vp.x)));
            nu_i32_t ymax
                = NU_MIN(vp.max.y, NU_MAX(v0vp.y, NU_MAX(v1vp.y, v2vp.y)));

            for (nu_i32_t y = ymin; y < ymax; ++y)
            {
                nu_f32_t *row_depth
                    = &((nu_f32_t *)(env->inst->memory
                                     + NUX_RAM_ZBUFFER))[y * NUX_SCREEN_WIDTH];
                nu_u8_t *row_pixel
                    = env->inst->memory + NUX_RAM_SCREEN + y * NUX_SCREEN_WIDTH;
                for (nu_i32_t x = xmin; x < xmax; ++x)
                {
                    // Compute weights
                    nu_i32_t w0 = pixel_coverage(v1vp, v2vp, x, y);
                    nu_i32_t w1 = pixel_coverage(v2vp, v0vp, x, y);
                    nu_i32_t w2 = pixel_coverage(v0vp, v1vp, x, y);

                    nu_bool_t included = NU_TRUE;
                    // included &= (w0 == 0) ? t0 : (w0 > 0);
                    // included &= (w1 == 0) ? t1 : (w1 > 0);
                    // included &= (w2 == 0) ? t2 : (w2 > 0);
                    included = (w0 >= 0 && w1 >= 0 && w2 >= 0);
                    if (!included)
                    {
                        continue;
                    }
                    // if (!(w0 >= 0 && w1 >= 0 && w2 >= 0))
                    // {
                    //     continue;
                    // }

                    nu_f32_t a = (nu_f32_t)w0 / (nu_f32_t)area;
                    nu_f32_t b = (nu_f32_t)w1 / (nu_f32_t)area;
                    nu_f32_t c = 1 - a - b;

                    a *= inv_vw0;
                    b *= inv_vw1;
                    c *= inv_vw2;

                    nu_f32_t inv_sum_abc = 1.0 / (a + b + c);
                    a *= inv_sum_abc;
                    b *= inv_sum_abc;
                    c *= inv_sum_abc;

                    // Depth test
                    nu_f32_t depth = (a * v0.z + b * v1.z + c * v2.z);
                    if (depth < row_depth[x])
                    {
                        row_depth[x] = depth;

                        // Texture sampling
                        // nu_f32_t px = (w0 * uv0.x + w1 * uv1.x + w2 *
                        // uv2.x)
                        //               * cube_texture_size.x;
                        // nu_f32_t py = (w0 * uv0.y + w1 * uv1.y + w2 *
                        // uv2.y)
                        //               * cube_texture_size.y;

                        nu_u32_t px
                            = (a * uv0.x + b * uv1.x + c * uv2.x) * texw;
                        nu_u32_t py
                            = (a * uv0.y + b * uv1.y + c * uv2.y) * texh;

                        nu_u32_t uvx = NU_MIN(texw - 1, px);
                        nu_u32_t uvy = NU_MIN(texh - 1, texh - 1 - py);

                        row_pixel[x] = nux_palc(env, tex[uvy * texw + uvx]);
                    }
                }
            }
        }
    }
}
