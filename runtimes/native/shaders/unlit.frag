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
    vec4 color;
    float dist_cam;
} fs_in;

const float dither4x4[16] = float[16]
(
     0.,  8.,  2.,  10.,
    12.,  4., 14.,   6.,
     3., 11.,  1.,   9.,
    15.,  7., 13.,   5.
);

const float dither8x8[64] = float[64]
(
     0., 32.,  8., 40.,  2., 34., 10., 42.,  
    48., 16., 56., 24., 50., 18., 58., 26.,  
    12., 44.,  4., 36., 14., 46.,  6., 38.,  
    60., 28., 52., 20., 62., 30., 54., 22.,  
     3., 35., 11., 43.,  1., 33.,  9., 41.,  
    51., 19., 59., 27., 49., 17., 57., 25.,  
    15., 47.,  7., 39., 13., 45.,  5., 37.,  
    63., 31., 55., 23., 61., 29., 53., 21.
);

void main()
{

    vec4 color = texture(texture0, fs_in.uv) * fs_in.color;
    float fog = clamp((fs_in.dist_cam - fog_near) / (fog_far - fog_near), 0, 1) * fog_density;
    int x = int(gl_FragCoord.x) % 8;
    int y = int(gl_FragCoord.y) % 8;
    float thres = (1 + dither8x8[x + y * 8]) / 64.;
    color.w *= (1 - fog);
    if (color.w >= thres)
        frag_color = mix(color, vec4(fog_color.xyz, 1), fog);
    else
        discard;
}
