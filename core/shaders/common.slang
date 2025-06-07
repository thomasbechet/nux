struct Constants
{
    float4x4 view;
    float4x4 proj;
    float4x4 model;
    uint2    canvas_size;
    uint2    screen_size;
    float    time;
    float3   _pad;
}

float4 mix(float4 a, float4 b, float c)
{
    return b * c + a * (1 - c);
}
