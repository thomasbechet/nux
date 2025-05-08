#version 330 core

in vec2 uv;
out vec4 color;

uniform usampler2D t_canvas;
uniform sampler2D t_colormap;

// vec2 uv_filtering(in vec2 uv, in vec2 texture_size)
// {
//     vec2 pixel = uv * texture_size;
//     vec2 seam = floor(pixel + 0.5);
//     vec2 dudv = fwidth(pixel);
//     vec2 rel = (pixel - seam) / dudv;
//     vec2 mid_pix = vec2(0.5);
//     pixel = seam + clamp(rel, -mid_pix, mid_pix);
//     return pixel / texture_size;
// }

void main()
{
    // vec2 surface_size = textureSize(t_surface, 0);
    // vec2 filtered_uv = uv_filtering(vec2(uv.x, 1 - uv.y), surface_size);
    vec2 uv = vec2(uv.x, 1 - uv.y);
    uint idx = texture(t_canvas, uv).r;
    color = texture(t_colormap, vec2(float(idx) / 255.0, 0));
}
