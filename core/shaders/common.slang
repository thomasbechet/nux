struct Constants
{
    float4x4 view;
    float4x4 proj;
    uint2    canvas_size;
    uint2    screen_size;
    float    time;
}

float4 mix(float4 a, float4 b, float c)
{
    return b * c + a * (1 - c);
}
