#version 330 core

out vec4 frag_color;

uniform sampler2D texture0;

layout(std140) uniform UBO {
    mat4 view;
    mat4 projection;
    vec4 color;
    vec4 fog_color;
    uvec2 viewport_size;
    float fog_density;
    float fog_near;
    float fog_far;
};

in VS_OUT {
    vec2 uv;
    vec3 color;
    float dist_cam;
} fs_in;

const float dither4x4[16] = float[16]
(
     0.,  8.,  2.,  10.,
    12.,  4., 14.,   6.,
     3., 11.,  1.,   9.,
    15.,  8., 13.,   5.
);

void main()
{

    vec4 color = texture(texture0, fs_in.uv) * vec4(fs_in.color, 1);
    float fog = clamp((fs_in.dist_cam - fog_near) / (fog_far - fog_near), 0, 1) * fog_density;
    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);
    float thres = 1. / (1 + dither4x4[x + y * 4]);
    if (thres * color.w > fog * color.w)
        frag_color = mix(color, vec4(fog_color.xyz, 1), fog);
        // frag_color = color;
    else
        discard;
}
