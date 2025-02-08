#version 330 core

out vec4 frag_color;

uniform sampler2D texture0;

in VS_OUT {
    vec2 uv;
    vec3 color;
} fs_in;

float near = 0.1; 
float far  = 500.0; 
  
float linear_depth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depth = gl_FragCoord.z;
    float fog = 1 - clamp(linear_depth(depth) / far, 0.0, 1.0);

    const vec4 fog_color = vec4(vec3(0.6), 1);
    vec4 color = texture(texture0, fs_in.uv) * vec4(fs_in.color, 1);
    frag_color = fog * color + (1.0 - fog) * fog_color; 
}
