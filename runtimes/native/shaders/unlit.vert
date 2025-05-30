#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_color;

layout(std140) uniform UBO {
    mat4 view;
    mat4 projection;
    vec4 color;
    vec4 fog_color;
    uvec2 viewport_size;
    float fog_density;
    float fog_near;
    float fog_far;
    bool is_volume;
};

uniform mat4 model;

out VS_OUT {
    vec2 uv;
    vec4 color;
    float dist_cam;
    vec3 world_pos;
} vs_out;

vec4 snap_vertex(in vec4 position)
{
    vec2 grid = vec2(viewport_size);
    position.xy = position.xy / position.w;
    position.xy = (floor(grid * position.xy) + 0.5) / grid;
    position.xy *= position.w;
    return position;
}

void main()
{
    vec4 world_pos = model * vec4(in_position, 1);
    vs_out.world_pos = world_pos.xyz;
    vec4 view_pos = view * world_pos;
    vs_out.dist_cam = length(view_pos);
    vec4 position = projection * view_pos;
    gl_Position = snap_vertex(position);

    vs_out.uv = in_uv;
    vs_out.color = vec4(in_color, 1) * color;
}
