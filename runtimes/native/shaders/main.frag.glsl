#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 22 0
layout(binding = 2)
uniform texture2D texture0_0;


#line 23
layout(binding = 3)
uniform sampler sampler_0;


#line 1867 1
layout(location = 0)
out vec4 entryPointParam_fragmentMain_0;


#line 33 0
void main()
{

#line 33
    entryPointParam_fragmentMain_0 = (texture(sampler2D(texture0_0,sampler_0), (vec2(uvec2(ivec2(0))))));

#line 33
    return;
}

