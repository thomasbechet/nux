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

void main()
{
    vec4 color = texture(texture0, fs_in.uv) * vec4(fs_in.color, 1);
    float fog = clamp((fs_in.dist_cam - fog_near) / (fog_far - fog_near), 0, 1) * fog_density;
    frag_color = mix(color, fog_color, fog);
}
