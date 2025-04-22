#version 330 core

out vec4 out_color;
in vec2 uv;

uniform sampler2D t_indices;
uniform sampler2D t_colormap;

void main()
{
    vec2 invuv = vec2(uv.x, 1 - uv.y);
    vec4 color_index = texture(t_indices, invuv);
    // vec4 color_index = texture(t_surface, invuv);
    out_color = texture(t_colormap, vec2(color_index.x, 0));
}
