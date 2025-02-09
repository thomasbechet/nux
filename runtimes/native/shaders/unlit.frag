#version 330 core

out vec4 frag_color;

uniform sampler2D texture0;

layout(std140) uniform UBO {
    mat4 view_projection;
    vec4 fog_color;
    uvec2 viewport_size;
    float fog_density;
    float fog_near;
    float fog_far;
};

in VS_OUT {
    vec2 uv;
    vec3 color;
} fs_in;
  
float linear_depth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depth = gl_FragCoord.z;
    float fog = clamp(linear_depth(depth, 0.01, 500) / fog_far, 0.0, 1.0) * fog_density;

    vec4 color = texture(texture0, fs_in.uv) * vec4(fs_in.color, 1);
    frag_color = mix(color, fog_color, fog);
}
