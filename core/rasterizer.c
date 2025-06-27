#include "internal.h"

void
nux_rectfill (nux_ctx_t *ctx,
              nux_i32_t  x0,
              nux_i32_t  y0,
              nux_i32_t  x1,
              nux_i32_t  y1,
              nux_u8_t   color)
{
    nux_i32_t minx = NUX_MIN(x0, x1);
    nux_i32_t miny = NUX_MIN(y0, y1);
    nux_i32_t maxx = NUX_MAX(x0, x1);
    nux_i32_t maxy = NUX_MAX(y0, y1);
    for (nux_i32_t y = miny; y <= maxy; ++y)
    {
        for (nux_i32_t x = minx; x <= maxx; ++x)
        {
            nux_pset(ctx, x, y, color);
        }
    }
}
void
nux_pset (nux_ctx_t *ctx, nux_i32_t x, nux_i32_t y, nux_u8_t color)
{
    if (x < 0 || x >= NUX_CANVAS_WIDTH || y < 0 || y >= NUX_CANVAS_HEIGHT)
    {
        return;
    }

    nux_u8_t *canvas                 = ctx->canvas;
    canvas[y * NUX_CANVAS_WIDTH + x] = nux_palc(ctx, color);
}
void
nux_graphics_line (nux_ctx_t *ctx,
                   nux_i32_t  x0,
                   nux_i32_t  y0,
                   nux_i32_t  x1,
                   nux_i32_t  y1,
                   nux_u8_t   c)
{
    nux_i32_t dx  = NUX_ABS(x1 - x0);
    nux_i32_t sx  = x0 < x1 ? 1 : -1;
    nux_i32_t dy  = -NUX_ABS(y1 - y0);
    nux_i32_t sy  = y0 < y1 ? 1 : -1;
    nux_i32_t err = dx + dy;
    nux_i32_t e2;
    for (;;)
    {
        nux_pset(ctx, x0, y0, c);
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
nux_trifill (nux_ctx_t *ctx,
             nux_i32_t  x0,
             nux_i32_t  y0,
             nux_i32_t  x1,
             nux_i32_t  y1,
             nux_i32_t  x2,
             nux_i32_t  y2,
             nux_u8_t   c)
{
    // v0.y <= v1.y <= v2.y
    nux_v2i_t v0 = nux_v2i(x0, y0);
    nux_v2i_t v1 = nux_v2i(x1, y1);
    nux_v2i_t v2 = nux_v2i(x2, y2);
    if (y0 == y1 && y0 == y2)
    {
        nux_graphics_line(ctx,
                          NUX_MIN(x0, NUX_MIN(x1, x2)),
                          y0,
                          NUX_MAX(x0, NUX_MAX(x1, x2)),
                          y0,
                          c);
        return;
    }
    if (v0.y > v1.y)
    {
        NUX_SWAP(v0, v1, nux_v2i_t);
    }
    if (v0.y > v2.y)
    {
        NUX_SWAP(v0, v2, nux_v2i_t);
    }
    if (v1.y > v2.y)
    {
        NUX_SWAP(v1, v2, nux_v2i_t);
    }

    // 3 bresenham iterations :
    //  v0 -> v1 (bresenham0) (check transition to v2)
    //  v1 -> v2 (bresenham1)
    //  v0 -> v2 (bresenham1) (longest iteration)

    nux_i32_t dx0  = NUX_ABS(v1.x - v0.x);
    nux_i32_t dx1  = NUX_ABS(v2.x - v0.x);
    nux_i32_t sx0  = v0.x < v1.x ? 1 : -1;
    nux_i32_t sx1  = v0.x < v2.x ? 1 : -1;
    nux_i32_t dy0  = -NUX_ABS(v1.y - v0.y);
    nux_i32_t dy1  = -NUX_ABS(v2.y - v0.y);
    nux_i32_t sy0  = v0.y < v1.y ? 1 : -1;
    nux_i32_t sy1  = v0.y < v2.y ? 1 : -1;
    nux_i32_t err0 = dx0 + dy0;
    nux_i32_t err1 = dx1 + dy1;
    nux_i32_t e20, e21;
    nux_i32_t curx0 = v0.x;
    nux_i32_t curx1 = v0.x;
    nux_i32_t cury0 = v0.y;
    nux_i32_t cury1 = v0.y;
    nux_b32_t lower = NUX_FALSE;

bresenham0:
    for (;;)
    {
        if (curx0 == v1.x && cury0 == v1.y && !lower)
        {
            // End of first iteration, transition to v1 -> v2
            dx0   = NUX_ABS(v2.x - v1.x);
            sx0   = v1.x < v2.x ? 1 : -1;
            dy0   = -NUX_ABS(v2.y - v1.y);
            sy0   = v1.y < v2.y ? 1 : -1;
            err0  = dx0 + dy0;
            curx0 = v1.x;
            cury0 = v1.y;
            lower = NUX_TRUE;
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
    for (nux_i32_t x = NUX_MIN(curx0, curx1); x < NUX_MAX(curx0, curx1); ++x)
    {
        nux_pset(ctx, x, cury1, c);
    }
    if (cury1 == v2.y)
    {
        return;
    }
    goto bresenham0;
}
void
nux_graphics_circle (
    nux_ctx_t *ctx, nux_i32_t xm, nux_i32_t ym, nux_i32_t r, nux_u8_t c)
{
    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do
    {
        nux_pset(ctx, xm - x, ym + y, c); /*   I. Quadrant */
        nux_pset(ctx, xm - y, ym - x, c); /*  II. Quadrant */
        nux_pset(ctx, xm + x, ym - y, c); /* III. Quadrant */
        nux_pset(ctx, xm + y, ym + x, c); /*  IV. Quadrant */
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
nux_graphics_rectangle (nux_ctx_t *ctx,
                        nux_i32_t  x0,
                        nux_i32_t  y0,
                        nux_i32_t  x1,
                        nux_i32_t  y1,
                        nux_u8_t   c)
{
    nux_i32_t xmin = NUX_MIN(x0, x1);
    nux_i32_t xmax = NUX_MAX(x0, x1);
    nux_i32_t ymin = NUX_MIN(y0, y1);
    nux_i32_t ymax = NUX_MAX(y0, y1);
    for (nux_i32_t x = xmin; x <= xmax; ++x)
    {
        nux_pset(ctx, x, y0, c);
        nux_pset(ctx, x, y1, c);
    }
    for (nux_i32_t y = ymin; y <= ymax; ++y)
    {
        nux_pset(ctx, x0, y, c);
        nux_pset(ctx, x1, y, c);
    }
}

// static inline nux_v4_t
// vertex_shader (const nux_m4_t transform, nux_f32_t x, nux_f32_t y, nux_f32_t
// z)
// {
//     nux_v4_t ret;
//     ret.x
//         = transform.x1 * x + transform.y1 * y + transform.z1 * z +
//         transform.w1;
//     ret.y
//         = transform.x2 * x + transform.y2 * y + transform.z2 * z +
//         transform.w2;
//     ret.z
//         = transform.x3 * x + transform.y3 * y + transform.z3 * z +
//         transform.w3;
//     ret.w
//         = transform.x4 * x + transform.y4 * y + transform.z4 * z +
//         transform.w4;
//     return ret;
// }
// static inline void
// clip_edge_near (nux_v4_t  v0,
//                 nux_v4_t  v1,
//                 nux_v2_t  uv0,
//                 nux_v2_t  uv1,
//                 nux_v4_t *vclip,
//                 nux_v2_t *uvclip)
// {
//     const nux_v4_t near_plane = nux_v4(0, 0, 1, 1);
//     nux_f32_t      d0         = nux_v4_dot(v0, near_plane);
//     nux_f32_t      d1         = nux_v4_dot(v1, near_plane);
//     nux_f32_t      s          = d0 / (d0 - d1);
//     *vclip                    = nux_v4_lerp(v0, v1, s);
//     *uvclip                   = nux_v2_lerp(uv0, uv1, s);
// }
// static nux_u32_t
// clip_triangle (nux_v4_t vertices[4], nux_v2_t uvs[4], nux_u32_t indices[6])
// {
//     // Default triangle output
//     indices[0] = 0;
//     indices[1] = 1;
//     indices[2] = 2;
//
//     // Compute outsides
//     nux_b32_t outside[3];
//     for (nux_u32_t i = 0; i < 3; ++i)
//     {
//         outside[i] = (vertices[i].w <= 0) || (vertices[i].z <
//         -vertices[i].w);
//     }
//
//     // Early test out
//     if ((outside[0] & outside[1] & outside[2]) != 0)
//     {
//         return 0;
//     }
//
//     // Early test in
//     if ((outside[0] | outside[1] | outside[2]) == 0)
//     {
//         return 3;
//     }
//
//     // Clip vertices
//     for (nux_u32_t i = 0; i < 3; ++i)
//     {
//         nux_v4_t *vec, *vec_prev, *vec_next;
//         nux_v2_t *uv, *uv_prev, *uv_next;
//         nux_b32_t out, out_prev, out_next;
//
//         vec = &vertices[i];
//         uv  = &uvs[i];
//         out = outside[i];
//
//         vec_next = &vertices[(i + 1) % 3];
//         uv_next  = &uvs[(i + 1) % 3];
//         out_next = outside[(i + 1) % 3];
//
//         vec_prev = &vertices[(i + 2) % 3];
//         uv_prev  = &uvs[(i + 2) % 3];
//         out_prev = outside[(i + 2) % 3];
//
//         if (out)
//         {
//             if (out_next) // 2 out : case 1
//             {
//                 clip_edge_near(*vec, *vec_prev, *uv, *uv_prev, vec, uv);
//             }
//             else if (out_prev) // 2 out : case 2
//             {
//                 clip_edge_near(*vec, *vec_next, *uv, *uv_next, vec, uv);
//             }
//             else // 1 out
//             {
//                 // Produce new vertex
//                 clip_edge_near(
//                     *vec, *vec_next, *uv, *uv_next, &vertices[3], &uvs[3]);
//                 indices[3] = i;
//                 indices[4] = 3; // New vertex
//                 indices[5] = (i + 1) % 3;
//
//                 // Clip existing vertex
//                 clip_edge_near(*vec, *vec_prev, *uv, *uv_prev, vec, uv);
//
//                 return 6;
//             }
//         }
//     }
//
//     return 3;
// }
//
// static inline nux_v2i_t
// pos_to_viewport (nu_b2i_t vp, nux_f32_t x, nux_f32_t y)
// {
//     nux_v2u_t size = nu_b2i_size(vp);
//     nux_i32_t px   = (x + 1) * 0.5 * (nux_f32_t)size.x;
//     nux_i32_t py   = size.y - (y + 1) * 0.5 * (nux_f32_t)size.y;
//     return nux_v2i(vp.min.x + px, vp.min.y + py);
// }
//
// static inline nux_i32_t
// pixel_coverage (nux_v2i_t a, nux_v2i_t b, nux_i32_t x, nux_i32_t y)
// {
//     return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
// }
//
// static inline nux_u16_t
// sample_texture (nux_ctx_t *ctx, nux_f32_t u, nux_f32_t v)
// {
//     nux_u32_t tx = NUX_MEMPTR(ctx->inst, NUX_RAM_TEXTURE_VIEW, nux_u32_t)[0];
//     nux_u32_t ty = NUX_MEMPTR(ctx->inst, NUX_RAM_TEXTURE_VIEW, nux_u32_t)[1];
//     nux_u32_t tw = NUX_MEMPTR(ctx->inst, NUX_RAM_TEXTURE_VIEW, nux_u32_t)[2];
//     nux_u32_t th = NUX_MEMPTR(ctx->inst, NUX_RAM_TEXTURE_VIEW, nux_u32_t)[3];
//
//     // Point based filtering
//     nux_i32_t x = (nux_i32_t)(u * (nux_f32_t)tw);
//     nux_i32_t y = (nux_i32_t)(v * (nux_f32_t)th);
//
//     // Texture wrapping
//     x = ((x % tw) + tw) % tw;
//     y = ((y % th) + th) % th;
//
//     // Inverse y coordinate
//     y = th - y - 1;
//
//     nux_u8_t *base = NUX_MEMPTR(ctx->inst, NUX_RAM_TEXTURE, nux_u8_t)
//                      + (ty * NUX_TEXTURE_WIDTH + tx);
//     return base[y * NUX_TEXTURE_WIDTH + x];
// }

// static void
// raster_fill_triangles (nux_ctx_t        ctx,
//                        const nux_f32_t *positions,
//                        const nux_f32_t *uvs,
//                        nux_u32_t        count,
//                        const nux_f32_t *m)
// {
//     const nux_f32_t *cameye
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_EYE, const nux_f32_t);
//     const nux_f32_t *camcenter
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_CENTER, const nux_f32_t);
//     const nux_f32_t *camup
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_UP, const nux_f32_t);
//     nux_f32_t        fov = NUX_MEMGET(ctx->inst, NUX_RAM_CAM_FOV, nux_f32_t);
//     const nux_u32_t *viewport
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_VIEWPORT, const nux_u32_t);
//
//     nux_m4_t view = nu_lookat(nux_v3(cameye[0], cameye[1], cameye[2]),
//                              nux_v3(camcenter[0], camcenter[1],
//                              camcenter[2]), nux_v3(camup[0], camup[1],
//                              camup[2]));
//     nux_m4_t proj = nu_perspective(
//         nu_radian(fov), (nux_f32_t)viewport[2] / viewport[3], 0.05, 300);
//     nux_m4_t  view_proj = nux_m4_mul(proj, view);
//     nux_m4_t  model     = nux_m4(m);
//     nux_m4_t  mvp       = nux_m4_mul(view_proj, model);
//     nu_b2i_t vp
//         = nu_b2i_xywh(viewport[0], viewport[1], viewport[2], viewport[3]);
//
//     // Iterate over triangles
//     for (nux_u32_t i = 0; i < count; i += 3)
//     {
//         // Apply vertex shader
//         nux_v4_t vertex_positions[4];
//         nux_v2_t vertex_uvs[4];
//
//         // Transform to world
//         for (nux_u32_t j = 0; j < 3; ++j)
//         {
//             // vertex_positions[j] = vertex_shader(mvp,
//             //                                     positions[(i + j) * 3 +
//             0],
//             //                                     positions[(i + j) * 3 +
//             1],
//             //                                     positions[(i + j) * 3 +
//             2]); vertex_positions[j] = nux_m4_mulv(model,
//                                              nux_v4(positions[(i + j) * 3 +
//                                              0],
//                                                    positions[(i + j) * 3 +
//                                                    1], positions[(i + j) * 3
//                                                    + 2], 1));
//         }
//
//         // Compute normal
//         nux_v3_t normal = nu_triangle_normal(nux_v3_v4(vertex_positions[0]),
//                                             nux_v3_v4(vertex_positions[1]),
//                                             nux_v3_v4(vertex_positions[2]));
//
//         // Transform to NDC
//         for (nux_u32_t j = 0; j < 3; ++j)
//         {
//             vertex_positions[j] = nux_m4_mulv(view_proj,
//             vertex_positions[j]);
//         }
//
//         // Read uvs
//         for (nux_u32_t j = 0; j < 3; ++j)
//         {
//             vertex_uvs[j].x = uvs[(i + j) * 2 + 0];
//             vertex_uvs[j].y = uvs[(i + j) * 2 + 1];
//         }
//
//         // Clip vertices
//         nux_u32_t indices[6];
//         nux_u32_t indices_count
//             = clip_triangle(vertex_positions, vertex_uvs, indices);
//         if (!indices_count)
//         {
//             continue;
//         }
//
//         // Perspective divide (NDC)
//         for (nux_u32_t i = 0; i < 4; i++)
//         {
//             vertex_positions[i].x /= vertex_positions[i].w;
//             vertex_positions[i].y /= vertex_positions[i].w;
//             vertex_positions[i].z /= vertex_positions[i].w;
//         }
//
//         // Iterate over clipped triangles
//         for (nux_u32_t v = 0; v < indices_count; v += 3)
//         {
//             nux_v4_t v0 = vertex_positions[indices[v + 0]];
//             nux_v4_t v1 = vertex_positions[indices[v + 1]];
//             nux_v4_t v2 = vertex_positions[indices[v + 2]];
//
//             nux_v2_t uv0 = vertex_uvs[indices[v + 0]];
//             nux_v2_t uv1 = vertex_uvs[indices[v + 1]];
//             nux_v2_t uv2 = vertex_uvs[indices[v + 2]];
//
//             nux_v2i_t v0vp = pos_to_viewport(vp, v0.x, v0.y);
//             nux_v2i_t v1vp = pos_to_viewport(vp, v1.x, v1.y);
//             nux_v2i_t v2vp = pos_to_viewport(vp, v2.x, v2.y);
//
//             nux_i32_t area = pixel_coverage(v0vp, v1vp, v2vp.x, v2vp.y);
//             if (area < 0)
//             {
//                 continue;
//             }
//
//             ctx->tricount += 1;
//
//             // Keep inv weights for perspective correction
//             nux_f32_t inv_vw0 = 1. / v0.w;
//             nux_f32_t inv_vw1 = 1. / v1.w;
//             nux_f32_t inv_vw2 = 1. / v2.w;
//
//             nux_i32_t xmin
//                 = NUX_MAX(vp.min.x, NUX_MIN(v0vp.x, NUX_MIN(v1vp.x,
//                 v2vp.x)));
//             nux_i32_t ymin
//                 = NUX_MAX(vp.min.y, NUX_MIN(v0vp.y, NUX_MIN(v1vp.y,
//                 v2vp.y)));
//             nux_i32_t xmax
//                 = NUX_MIN(vp.max.x, NUX_MAX(v0vp.x, NUX_MAX(v1vp.x,
//                 v2vp.x)));
//             nux_i32_t ymax
//                 = NUX_MIN(vp.max.y, NUX_MAX(v0vp.y, NUX_MAX(v1vp.y,
//                 v2vp.y)));
//
//             for (nux_i32_t y = ymin; y < ymax; ++y)
//             {
//                 nux_f32_t *row_depth
//                     = &((nux_f32_t *)(ctx->inst->state
//                                      + NUX_RAM_ZBUFFER))[y *
//                                      NUX_CANVAS_WIDTH];
//                 nu_u8_t *row_pixel
//                     = ctx->inst->state + NUX_RAM_CANVAS + y *
//                     NUX_CANVAS_WIDTH;
//                 for (nux_i32_t x = xmin; x <= xmax; ++x)
//                 {
//                     // Compute weights
//                     nux_i32_t w0 = pixel_coverage(v1vp, v2vp, x, y);
//                     nux_i32_t w1 = pixel_coverage(v2vp, v0vp, x, y);
//                     nux_i32_t w2 = pixel_coverage(v0vp, v1vp, x, y);
//
//                     nux_b32_t included = (w0 >= 0 && w1 >= 0 && w2 >= 0);
//                     if (!included)
//                     {
//                         continue;
//                     }
//
//                     nux_f32_t a = (nux_f32_t)w0 / (nux_f32_t)area;
//                     nux_f32_t b = (nux_f32_t)w1 / (nux_f32_t)area;
//                     nux_f32_t c = 1 - a - b;
//
//                     nux_f32_t depth = (a * v0.z + b * v1.z + c * v2.z);
//
//                     a *= inv_vw0;
//                     b *= inv_vw1;
//                     c *= inv_vw2;
//
//                     nux_f32_t inv_sum_abc = 1.0 / (a + b + c);
//                     a *= inv_sum_abc;
//                     b *= inv_sum_abc;
//                     c *= inv_sum_abc;
//
//                     // Depth test
//                     nux_f32_t *pdepth = &(
//                         (nux_f32_t *)(ctx->inst->state
//                                      + NUX_RAM_ZBUFFER))[y * NUX_CANVAS_WIDTH
//                                                          + x];
//                     if (depth < *pdepth)
//                     {
//                         // row_depth[x] = depth;
//                         *pdepth = depth;
//
//                         nux_f32_t u = a * uv0.x + b * uv1.x + c * uv2.x;
//                         nux_f32_t v = a * uv0.y + b * uv1.y + c * uv2.y;
//                         nux_u16_t c = sample_texture(ctx, u, v);
//
//                         // nux_f32_t t = nux_time(ctx);
//                         // nux_v3_t  sun
//                         //     = nux_v3_normalize(nux_v3(nu_sin(t), 1,
//                         //     nu_cos(t)));
//                         // nux_f32_t dot = NUX_MAX(0.5, nux_v3_dot(normal,
//                         sun));
//                         // c            = blend_color(c, 0, dot);
//
//                         NUX_ENCODE_COLOR(ctx->inst->state + NUX_RAM_CANVAS,
//                                          y * NUX_CANVAS_WIDTH + x,
//                                          c);
//                     }
//                 }
//             }
//
//             // {
//             //     nux_u32_t x0 = NUX_CLAMP(v0vp.x, 0, NUX_SCREEN_WIDTH);
//             //     nux_u32_t x1 = NUX_CLAMP(v1vp.x, 0, NUX_SCREEN_WIDTH);
//             //     nux_u32_t x2 = NUX_CLAMP(v2vp.x, 0, NUX_SCREEN_WIDTH);
//             //     nux_u32_t y0 = NUX_CLAMP(v0vp.y, 0, NUX_SCREEN_HEIGHT);
//             //     nux_u32_t y1 = NUX_CLAMP(v1vp.y, 0, NUX_SCREEN_HEIGHT);
//             //     nux_u32_t y2 = NUX_CLAMP(v2vp.y, 0, NUX_SCREEN_HEIGHT);
//             //     nux_line(ctx, x0, y0, x1, y1, 0);
//             //     nux_line(ctx, x0, y0, x2, y2, 0);
//             //     nux_line(ctx, x1, y1, x2, y2, 0);
//             // }
//         }
//     }
// }
// static void
// raster_wire_triangles (nux_ctx_t        ctx,
//                        const nux_f32_t *positions,
//                        const nux_f32_t *uvs,
//                        nux_u32_t        count,
//                        const nux_f32_t *m)
// {
//     const nux_f32_t *cameye
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_EYE, const nux_f32_t);
//     const nux_f32_t *camcenter
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_CENTER, const nux_f32_t);
//     const nux_f32_t *camup
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_UP, const nux_f32_t);
//     nux_f32_t        fov = NUX_MEMGET(ctx->inst, NUX_RAM_CAM_FOV, nux_f32_t);
//     const nux_u32_t *viewport
//         = NUX_MEMPTR(ctx->inst, NUX_RAM_CAM_VIEWPORT, const nux_u32_t);
//
//     nux_m4_t view = nux_lookat(nux_v3(cameye[0], cameye[1], cameye[2]),
//                                nux_v3(camcenter[0], camcenter[1],
//                                camcenter[2]), nux_v3(camup[0], camup[1],
//                                camup[2]));
//     nux_m4_t proj = nux_perspective(
//         nux_radian(fov), (nux_f32_t)viewport[2] / viewport[3], 0.05, 300);
//     nux_m4_t  view_proj = nux_m4_mul(proj, view);
//     nux_m4_t  model     = nux_m4(m);
//     nux_m4_t  mvp       = nux_m4_mul(view_proj, model);
//     nux_b2i_t vp
//         = nux_b2i_xywh(viewport[0], viewport[1], viewport[2], viewport[3]);
//
//     // Iterate over triangles
//     for (nux_u32_t i = 0; i < count; i += 3)
//     {
//         // Apply vertex shader
//         nux_v4_t vertex_positions[4];
//         nux_v2_t vertex_uvs[4];
//
//         // Transform to world
//         for (nux_u32_t j = 0; j < 3; ++j)
//         {
//             vertex_positions[j] = nux_m4_mulv(model,
//                                               nux_v4(positions[(i + j) * 3 +
//                                               0],
//                                                      positions[(i + j) * 3 +
//                                                      1], positions[(i + j) *
//                                                      3 + 2], 1));
//         }
//
//         // Transform to NDC
//         for (nux_u32_t j = 0; j < 3; ++j)
//         {
//             vertex_positions[j] = nux_m4_mulv(view_proj,
//             vertex_positions[j]);
//         }
//
//         // Clip vertices
//         nux_u32_t indices[6];
//         nux_u32_t indices_count
//             = clip_triangle(vertex_positions, vertex_uvs, indices);
//         if (!indices_count)
//         {
//             continue;
//         }
//
//         // Perspective divide (NDC)
//         for (nux_u32_t i = 0; i < 4; i++)
//         {
//             vertex_positions[i].x /= vertex_positions[i].w;
//             vertex_positions[i].y /= vertex_positions[i].w;
//             vertex_positions[i].z /= vertex_positions[i].w;
//         }
//
//         // Iterate over clipped triangles
//         for (nux_u32_t v = 0; v < indices_count; v += 3)
//         {
//             nux_v4_t v0 = vertex_positions[indices[v + 0]];
//             nux_v4_t v1 = vertex_positions[indices[v + 1]];
//             nux_v4_t v2 = vertex_positions[indices[v + 2]];
//
//             nux_v2i_t v0vp = pos_to_viewport(vp, v0.x, v0.y);
//             nux_v2i_t v1vp = pos_to_viewport(vp, v1.x, v1.y);
//             nux_v2i_t v2vp = pos_to_viewport(vp, v2.x, v2.y);
//
//             nux_i32_t area = pixel_coverage(v0vp, v1vp, v2vp.x, v2vp.y);
//             if (area < 0)
//             {
//                 continue;
//             }
//
//             nux_u32_t x0 = NUX_CLAMP(v0vp.x, 0, NUX_CANVAS_WIDTH);
//             nux_u32_t x1 = NUX_CLAMP(v1vp.x, 0, NUX_CANVAS_WIDTH);
//             nux_u32_t x2 = NUX_CLAMP(v2vp.x, 0, NUX_CANVAS_WIDTH);
//             nux_u32_t y0 = NUX_CLAMP(v0vp.y, 0, NUX_CANVAS_HEIGHT);
//             nux_u32_t y1 = NUX_CLAMP(v1vp.y, 0, NUX_CANVAS_HEIGHT);
//             nux_u32_t y2 = NUX_CLAMP(v2vp.y, 0, NUX_CANVAS_HEIGHT);
//             nux_line(ctx, x0, y0, x1, y1, 7);
//             nux_line(ctx, x0, y0, x2, y2, 7);
//             nux_line(ctx, x1, y1, x2, y2, 7);
//         }
//     }
// }

void
nux_mesh (nux_ctx_t       *ctx,
          const nux_f32_t *positions,
          const nux_f32_t *uvs,
          nux_u32_t        count,
          const nux_f32_t *m)
{
}
void
nux_mesh_wire (nux_ctx_t       *ctx,
               const nux_f32_t *positions,
               const nux_f32_t *uvs,
               nux_u32_t        count,
               const nux_f32_t *m)
{
}
void
nux_draw_cube (nux_ctx_t       *ctx,
               nux_f32_t        sx,
               nux_f32_t        sy,
               nux_f32_t        sz,
               const nux_f32_t *m)
{
    // const nu_b3_t box = nu_b3(NU_V3_ZEROS, nux_v3(sx / 2, sy / 2, sz / 2));
    //
    // const nux_v3_t v0 = nux_v3(box.min.x, box.min.y, box.min.z);
    // const nux_v3_t v1 = nux_v3(box.max.x, box.min.y, box.min.z);
    // const nux_v3_t v2 = nux_v3(box.max.x, box.min.y, box.max.z);
    // const nux_v3_t v3 = nux_v3(box.min.x, box.min.y, box.max.z);
    //
    // const nux_v3_t v4 = nux_v3(box.min.x, box.max.y, box.min.z);
    // const nux_v3_t v5 = nux_v3(box.max.x, box.max.y, box.min.z);
    // const nux_v3_t v6 = nux_v3(box.max.x, box.max.y, box.max.z);
    // const nux_v3_t v7 = nux_v3(box.min.x, box.max.y, box.max.z);
    //
    // const nux_v3_t positions[]
    //     = { v0, v1, v2, v2, v3, v0, v4, v6, v5, v6, v4, v7,
    //         v0, v3, v7, v7, v4, v0, v1, v5, v6, v6, v2, v1,
    //         v0, v4, v5, v5, v1, v0, v3, v2, v6, v6, v7, v3 };
    //
    // const nux_v2_t uvs[] = {
    //     { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
    //     { { 0, 0 } }, { { 0, 0 } }, { { 1, 1 } }, { { 1, 0 } }, { { 1, 1 } },
    //     { { 0, 0 } }, { { 0, 1 } }, { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } },
    //     { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 1, 0 } },
    //     { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } }, { { 0, 0 } }, { { 0, 0 } },
    //     { { 0, 1 } }, { { 1, 1 } }, { { 1, 1 } }, { { 1, 0 } }, { { 0, 0 } },
    //     { { 0, 0 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 1 } }, { { 0, 1 } },
    //     { { 0, 0 } },
    // };
    //
    // nux_mesh(ctx,
    //          (const nux_f32_t *)positions,
    //          (const nux_f32_t *)uvs,
    //          NUX_ARRAY_SIZE(positions),
    //          m);
}
void
nux_draw_plane (nux_ctx_t *ctx, nux_f32_t w, nux_f32_t h, const nux_f32_t *m)
{
    const nux_v3_t v0 = nux_v3(0, 0, 0);
    const nux_v3_t v1 = nux_v3(w, 0, 0);
    const nux_v3_t v2 = nux_v3(w, 0, h);
    const nux_v3_t v3 = nux_v3(0, 0, h);

    const nux_v3_t positions[] = {
        // v0, v1, v2, v2, v3, v0,
        v0, v2, v1, v3, v2, v0,
    };

    const nux_v2_t uvs[] = { { { 0, 0 } }, { { 1, 1 } }, { { 1, 0 } },
                             { { 0, 1 } }, { { 1, 1 } }, { { 0, 0 } } };

    nux_mesh(ctx,
             (const nux_f32_t *)positions,
             (const nux_f32_t *)uvs,
             NUX_ARRAY_SIZE(positions),
             m);
}
