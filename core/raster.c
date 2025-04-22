#include "internal.h"

void
nux_fill (nux_env_t env,
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
    env->inst->memory[NUX_RAM_SCREEN + y * NUX_SCREEN_WIDTH + x] = color;
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
static nu_i32_t
coverage (nu_v2i_t a, nu_v2i_t b, nu_v2i_t c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

// void
// nux_triangle (nux_env_t env,
//               nux_i32_t x0,
//               nux_i32_t y0,
//               nux_f32_t z0,
//               nux_i32_t x1,
//               nux_i32_t y1,
//               nux_f32_t z1,
//               nux_i32_t x2,
//               nux_i32_t y2,
//               nux_f32_t z2,
//               nux_u8_t  c)
// {
//     // v0.y <= v1.y <= v2.y
//     nu_v2i_t v0 = nu_v2i(x0, y0);
//     nu_v2i_t v1 = nu_v2i(x1, y1);
//     nu_v2i_t v2 = nu_v2i(x2, y2);
//     if (y0 == y1 && y0 == y2)
//     {
//         nux_line(env,
//                  NU_MIN(x0, NU_MIN(x1, x2)),
//                  y0,
//                  NU_MAX(x0, NU_MAX(x1, x2)),
//                  y0,
//                  c);
//         return;
//     }
//     if (v0.y > v1.y)
//     {
//         NU_SWAP(v0, v1, nu_v2i_t);
//     }
//     if (v0.y > v2.y)
//     {
//         NU_SWAP(v0, v2, nu_v2i_t);
//     }
//     if (v1.y > v2.y)
//     {
//         NU_SWAP(v1, v2, nu_v2i_t);
//     }
//
//     // 3 bresenham iterations :
//     //  v0 -> v1 (bresenham0) (check transition to v2)
//     //  v1 -> v2 (bresenham1)
//     //  v0 -> v2 (bresenham1) (longest iteration)
//
//     nu_i32_t  dx0  = NU_ABS(v1.x - v0.x);
//     nu_i32_t  dx1  = NU_ABS(v2.x - v0.x);
//     nu_i32_t  sx0  = v0.x < v1.x ? 1 : -1;
//     nu_i32_t  sx1  = v0.x < v2.x ? 1 : -1;
//     nu_i32_t  dy0  = -NU_ABS(v1.y - v0.y);
//     nu_i32_t  dy1  = -NU_ABS(v2.y - v0.y);
//     nu_i32_t  sy0  = v0.y < v1.y ? 1 : -1;
//     nu_i32_t  sy1  = v0.y < v2.y ? 1 : -1;
//     nu_i32_t  err0 = dx0 + dy0;
//     nu_i32_t  err1 = dx1 + dy1;
//     nu_i32_t  e20, e21;
//     nu_i32_t  curx0 = v0.x;
//     nu_i32_t  curx1 = v0.x;
//     nu_i32_t  cury0 = v0.y;
//     nu_i32_t  cury1 = v0.y;
//     nu_bool_t lower = NU_FALSE;
//
//     nu_f32_t pixel_coverage = coverage(v0, v1, v2);
//     if (pixel_coverage < 0)
//     {
//         return;
//     }
//
// bresenham0:
//     for (;;)
//     {
//         if (curx0 == v1.x && cury0 == v1.y && !lower)
//         {
//             // End of first iteration, transition to v1 -> v2
//             dx0   = NU_ABS(v2.x - v1.x);
//             sx0   = v1.x < v2.x ? 1 : -1;
//             dy0   = -NU_ABS(v2.y - v1.y);
//             sy0   = v1.y < v2.y ? 1 : -1;
//             err0  = dx0 + dy0;
//             curx0 = v1.x;
//             cury0 = v1.y;
//             lower = NU_TRUE;
//         }
//         e20 = 2 * err0;
//         if (e20 >= dy0)
//         {
//             err0 += dy0;
//             curx0 += sx0;
//         }
//         if (e20 <= dx0)
//         {
//             err0 += dx0;
//             cury0 += sy0;
//             goto bresenham1;
//         }
//     }
//
// bresenham1:
//     for (;;)
//     {
//         e21 = 2 * err1;
//         if (e21 >= dy1)
//         {
//             err1 += dy1;
//             curx1 += sx1;
//         }
//         if (e21 <= dx1)
//         {
//             err1 += dx1;
//             cury1 += sy1;
//             goto processline;
//         }
//     }
//
// processline:
//     for (nu_i32_t x = NU_MIN(curx0, curx1); x < NU_MAX(curx0, curx1); ++x)
//     {
//         if (x < 0 || x >= NUX_SCREEN_WIDTH || cury1 < 0
//             || cury1 >= NUX_SCREEN_HEIGHT)
//         {
//             continue;
//         }
//
//         // float inv_vw0 = 1.0f / v0.w;
//         // float inv_vw1 = 1.0f / v1.w;
//         // float inv_vw2 = 1.0f / v2.w;
//
//         nu_v2i_t sample = nu_v2i(x, cury1);
//         nu_f32_t w0     = coverage(v1, v2, sample);
//         nu_f32_t w1     = coverage(v2, v0, sample);
//         // nu_f32_t w2     = coverage(v0, v1, sample);
//
//         const nu_f32_t area_inv = 1.0f / pixel_coverage;
//         w0 *= area_inv;
//         w1 *= area_inv;
//         nu_f32_t w2 = 1.0f - w0 - w1;
//
//         nu_f32_t depth = (w0 * z0 + w1 * z1 + w2 * z2);
//         if (depth < nux_zget(env, x, cury1))
//         {
//             nux_zset(env, x, cury1, depth);
//             nux_pset(env, x, cury1, c);
//         }
//     }
//     if (cury1 == v2.y)
//     {
//         return;
//     }
//     goto bresenham0;
// }
static inline void
fill_triangle (nux_env_t env,
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
// static inline void
// fill_bottom_triangle (
//     nux_env_t env, nu_v2i_t v0, nu_v2i_t v1, nu_v2i_t v2, nux_u8_t c)
// {
//     nux_f32_t invslope1 = (nux_f32_t)(v1.x - v0.x) / (v1.y - v0.y);
//     nux_f32_t invslope2 = (nux_f32_t)(v2.x - v0.x) / (v2.y - v0.y);
//
//     nux_f32_t curx1 = v0.x;
//     nux_f32_t curx2 = v0.x;
//
//     for (nux_i32_t y = v0.y; y <= v1.y; ++y)
//     {
//         nux_line(env, curx1, y, curx2, y, c);
//         curx1 += invslope1;
//         curx2 += invslope2;
//     }
// }
// static inline void
// fill_top_triangle (
//     nux_env_t env, nu_v2i_t v0, nu_v2i_t v1, nu_v2i_t v2, nux_u8_t c)
// {
//     nux_f32_t invslope1 = (nux_f32_t)(v2.x - v0.x) / (v2.y - v0.y);
//     nux_f32_t invslope2 = (nux_f32_t)(v2.x - v1.x) / (v2.y - v1.y);
//
//     nux_f32_t curx1 = v2.x;
//     nux_f32_t curx2 = v2.x;
//
//     for (nux_i32_t y = v2.y; y > v0.y; y--)
//     {
//         nux_line(env, curx1, y, curx2, y, c);
//         curx1 -= invslope1;
//         curx2 -= invslope2;
//     }
// }
void
nux_filltri (nux_env_t env,
             nux_i32_t x0,
             nux_i32_t y0,
             nux_i32_t x1,
             nux_i32_t y1,
             nux_i32_t x2,
             nux_i32_t y2,
             nux_u8_t  c)
{
    fill_triangle(env, x0, y0, x1, y1, x2, y2, c);
    return;

    // nu_v2i_t v0 = nu_v2i(x0, y0);
    // nu_v2i_t v1 = nu_v2i(x1, y1);
    // nu_v2i_t v2 = nu_v2i(x2, y2);
    // if (y0 == y1 && y0 == y2)
    // {
    //     return;
    // }
    // if (y0 > y1)
    // {
    //     NU_SWAP(v0, v1, nu_v2i_t);
    // }
    // if (y0 > y2)
    // {
    //     NU_SWAP(v0, v2, nu_v2i_t);
    // }
    // if (y1 > y2)
    // {
    //     NU_SWAP(v1, v2, nu_v2i_t);
    // }
    //
    // if (v1.y == v2.y)
    // {
    //     fill_bottom_triangle(env, v0, v1, v2, c);
    // }
    // else if (v0.y == v1.y)
    // {
    //     fill_top_triangle(env, v0, v1, v2, c);
    // }
    // else
    // {
    //     nu_v2i_t v3 = nu_v2i(
    //         (nux_i32_t)(v0.x
    //                     + ((nux_f32_t)(v1.y - v0.y) / (nux_f32_t)(v2.y -
    //                     v0.y))
    //                           * (nux_f32_t)(v2.x - v0.x)),
    //         v1.y);
    //     fill_bottom_triangle(env, v0, v1, v3, c);
    //     fill_top_triangle(env, v1, v3, v2, c);
    // }
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

static inline nu_v4_t
vertex_shader (const nu_m4_t transform, nu_v3_t position)
{
    nu_v4_t ret;
    ret.x = transform.x1 * position.x + transform.y1 * position.y
            + transform.z1 * position.z + transform.w1;
    ret.y = transform.x2 * position.x + transform.y2 * position.y
            + transform.z2 * position.z + transform.w2;
    ret.z = transform.x3 * position.x + transform.y3 * position.y
            + transform.z3 * position.z + transform.w3;
    ret.w = transform.x4 * position.x + transform.y4 * position.y
            + transform.z4 * position.z + transform.w4;
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
                indices[3] = i;
                indices[4] = 3; // New vertex
                indices[5] = (i + 1) % 3;

                // Clip existing vertex
                clip_edge_near(*vec, *vec_prev, *uv, *uv_prev, vec, uv);
                // clip_edge_near(*vec, *vec_prev, vec);

                return 6;
            }
        }
    }

    return 3;
}

static inline nu_v2i_t
pos_to_viewport (nu_u32_t vpw, nu_u32_t vph, nu_f32_t x, nu_f32_t y)
{
    nu_i32_t px = (x + 1) * 0.5 * vpw;
    nu_i32_t py = vph - ((y + 1) * 0.5 * vph);
    return nu_v2i(px, py);
}

static inline nu_i32_t
pixel_coverage (nu_v2i_t a, nu_v2i_t b, nu_i32_t x, nu_i32_t y)
{
    return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
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

    const nu_u8_t cube_texture[16]
        = { 1, 2, 3, 2, 3, 2, 1, 2, 1, 0, 5, 6, 1, 3, 3, 0 };
    const nu_v2u_t cube_texture_size = nu_v2u(4, 4);

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
        nu_u32_t indices[6];
        nu_u32_t indices_count = clip_triangle(vertices, uvs, indices);
        if (!indices_count)
        {
            continue;
        }

        // Perspective divide (NDC)
        for (nu_u32_t i = 0; i < 4; i++)
        {
            vertices[i].x /= vertices[i].w;
            vertices[i].y /= vertices[i].w;
            vertices[i].z /= vertices[i].w;
        }

        // Iterate over clipped triangles
        for (nu_u32_t v = 0; v < indices_count; v += 3)
        {
            nu_v4_t v0 = vertices[indices[v + 0]];
            nu_v4_t v1 = vertices[indices[v + 1]];
            nu_v4_t v2 = vertices[indices[v + 2]];

            nu_v2i_t v0vp = pos_to_viewport(
                NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT, v0.x, v0.y);
            nu_v2i_t v1vp = pos_to_viewport(
                NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT, v1.x, v1.y);
            nu_v2i_t v2vp = pos_to_viewport(
                NUX_SCREEN_WIDTH, NUX_SCREEN_HEIGHT, v2.x, v2.y);

            nu_i32_t area = pixel_coverage(v0vp, v1vp, v2vp.x, v2vp.y);
            if (area <= 0)
            {
                continue;
            }

            nu_v2_t uv0 = uvs[indices[v + 0]];
            nu_v2_t uv1 = uvs[indices[v + 1]];
            nu_v2_t uv2 = uvs[indices[v + 2]];

            // Keep inv weights for perspective correction
            nu_f32_t inv_vw0 = 1. / v0.w;
            nu_f32_t inv_vw1 = 1. / v1.w;
            nu_f32_t inv_vw2 = 1. / v2.w;

            nu_i32_t xmin = NU_MAX(0, NU_MIN(v0vp.x, NU_MIN(v1vp.x, v2vp.x)));
            nu_i32_t ymin = NU_MAX(0, NU_MIN(v0vp.y, NU_MIN(v1vp.y, v2vp.y)));
            nu_i32_t xmax = NU_MIN(NUX_SCREEN_WIDTH,
                                   NU_MAX(v0vp.x, NU_MAX(v1vp.x, v2vp.x)));
            nu_i32_t ymax = NU_MIN(NUX_SCREEN_HEIGHT,
                                   NU_MAX(v0vp.y, NU_MAX(v1vp.y, v2vp.y)));

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

                        nu_u32_t px = (a * uv0.x + b * uv1.x + c * uv2.x)
                                      * cube_texture_size.x;
                        nu_u32_t py = (a * uv0.y + b * uv1.y + c * uv2.y)
                                      * cube_texture_size.y;

                        nu_v2u_t texsize = cube_texture_size;

                        nu_u32_t uvx = NU_MIN(texsize.x - 1, px);
                        nu_u32_t uvy
                            = NU_MIN(texsize.y - 1, texsize.y - 1 - py);

                        row_pixel[x] = cube_texture[uvy * texsize.x + uvx];
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

#ifdef NUX_BENCHMARK
#include <time.h>
#endif

void
nux_render_cubes (nux_env_t env, const nux_f32_t *view_proj)
{
#ifdef NUX_BENCHMARK
    clock_t t;
    t = clock();
#endif

    nu_m4_t         vp         = nu_m4(view_proj);
    const nu_bool_t stresstest = NU_TRUE;
    const nu_size_t size       = 20;
    const nu_f32_t  space      = 5;
    for (nu_u32_t x = 0; x < size; ++x)
    {
        for (nu_u32_t y = 0; y < size; ++y)
        {
            for (nu_u32_t z = 0; z < size; ++z)
            {
                nu_m4_t model = nu_m4_identity();
                model         = nu_m4_mul(
                    model,
                    nu_m4_translate(nu_v3(x * space, y * space, z * space)));
                model = nu_m4_mul(model, nu_m4_scale(nu_v3s(3)));
                model = nu_m4_mul(model,
                                  nu_m4_rotate_y(nu_radian(nux_time(env) * 0)));
                render_cube(env, vp, model);
            }
        }
    }

#ifdef NUX_BENCHMARK
    static double sum   = 0;
    static int    frame = 0;
    t                   = clock() - t;
    sum += ((double)t) / CLOCKS_PER_SEC; // in seconds
    ++frame;
    if (frame == 100)
    {
        printf("%lf\n", sum / 100);
        frame = 0;
        sum   = 0;
        exit(0);
    }
#endif
}
