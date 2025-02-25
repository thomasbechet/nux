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
    bool is_volume;
};

in VS_OUT {
    vec2 uv;
    vec4 color;
    float dist_cam;
    vec3 world_pos;
} fs_in;

const float dither4x4[16] = float[16](
        0., 8., 2., 10.,
        12., 4., 14., 6.,
        3., 11., 1., 9.,
        15., 7., 13., 5.
    );

const float dither8x8[64] = float[64](
        0., 32., 8., 40., 2., 34., 10., 42.,
        48., 16., 56., 24., 50., 18., 58., 26.,
        12., 44., 4., 36., 14., 46., 6., 38.,
        60., 28., 52., 20., 62., 30., 54., 22.,
        3., 35., 11., 43., 1., 33., 9., 41.,
        51., 19., 59., 27., 49., 17., 57., 25.,
        15., 47., 7., 39., 13., 45., 5., 37.,
        63., 31., 55., 23., 61., 29., 53., 21.
    );

vec2 ray_sphere(
    in vec3 ray_origin,
    in vec3 ray_direction,
    in vec3 sphere_center,
    in float sphere_radius
) {
    const float FLT_MAX = 10000000.0f;
    vec3 offset = ray_origin - sphere_center;
    float a = 1.0; // dot(ray_direction, rayDirection);
    float b = 2.0 * dot(offset, ray_direction);
    float c = dot(offset, offset) - sphere_radius * sphere_radius;
    float d = b * b - 4.0 * a * c; // Discriminant from quadratic formula

    // d < 0 => 0 point
    // d = 1 => 1 point
    // d > 0 => 2 points
    if (d > 0.0) {
        float s = sqrt(d);
        float dst_to_sphere_near = max(0.0, (-b - s) / (2.0 * a));
        float dst_to_sphere_far = (-b + s) / (2.0 * a);

        // Ignore intersections behind the ray
        if (dst_to_sphere_far >= 0.0) {
            return vec2(dst_to_sphere_near, dst_to_sphere_far - dst_to_sphere_near);
        }
    }

    // No intersection
    return vec2(FLT_MAX, 0.0);
}

float density_at_point(
    in vec3 density_sample_point
) {
    const float atmosphere_radius = 60.0;
    const float planet_radius = 50.0;
    const vec3 planet_center = vec3(0);
    const float density_falloff = 1;
    float height_above_surface = length(density_sample_point - planet_center) - planet_radius;
    float height01 = height_above_surface / (atmosphere_radius - planet_radius);
    float local_density = exp(-height01 * density_falloff) * (1.0 - height01);
    return local_density;
}

float optical_depth(
    in vec3 ray_origin,
    in vec3 ray_direction,
    in float ray_length
) {
    const int OPTICAL_DEPTH_POINT_COUNT = 10;
    vec3 density_sample_point = ray_origin;
    float step_size = ray_length / float(OPTICAL_DEPTH_POINT_COUNT - 1);
    float optical_depth = 0.0;

    for (int i = 0; i < OPTICAL_DEPTH_POINT_COUNT; i++) {
        float localDensity = density_at_point(density_sample_point);
        optical_depth += localDensity * step_size;
        density_sample_point += ray_direction * step_size;
    }

    return optical_depth;
}

vec3 calculate_light(
    in vec3 ray_origin,
    in vec3 ray_direction,
    in float ray_length,
    in vec3 original_color
) {
    const int SCATTER_POINT_COUNT = 10;
    const vec3 sun_position = vec3(100000);
    const vec3 planet_center = vec3(0);
    const float atmosphere_radius = 10;
    const vec3 scatter_rgb = vec3(1, 1, 1);

    vec3 dir_to_sun = normalize(sun_position - planet_center);
    vec3 scatter_point = ray_origin;
    float step_size = ray_length / float(SCATTER_POINT_COUNT - 1);
    vec3 scatter_light = vec3(0.0, 0.0, 0.0);

    float view_ray_optical_depth = 0.0;
    for (int i = 0; i < SCATTER_POINT_COUNT; i++) {
        float sun_ray_length = ray_sphere(scatter_point, dir_to_sun, planet_center, atmosphere_radius).y;
        float sun_ray_optical_depth = optical_depth(scatter_light, dir_to_sun, sun_ray_length);
        view_ray_optical_depth = optical_depth(scatter_point, -ray_direction, step_size * float(i));
        float local_density = density_at_point(scatter_point);
        vec3 transmittance = exp(-(sun_ray_optical_depth + view_ray_optical_depth) * scatter_rgb);

        scatter_light += local_density * step_size * transmittance * scatter_rgb;
        scatter_point += ray_direction * step_size;
    }

    float original_color_transmittance = exp(-view_ray_optical_depth);
    return original_color_transmittance * original_color + scatter_light;
}

void main()
{
    if (is_volume)
    {
        // vec3 original_color = texture2D(colorBuffer, gl_FragCoord.xy / resolution.xy).xyz;
        vec3 original_color = vec3(0);
        const vec3 planet_center = vec3(0);
        const float planet_radius = 50;
        const float atmosphere_radius = 60;
        // float depthNonLinear = texture2D(depthBuffer, gl_FragCoord.xy / resolution.xy).r;
        // float depth          = linearizeDepth(depthNonLinear, 0.1, 1000.0);

        vec3 view_pos = vec3(transpose(view));

        vec3 ray_origin = view_pos;
        vec3 ray_dir = normalize(fs_in.world_pos.xyz - view_pos);

        vec2 planet_hit_info = ray_sphere(ray_origin, ray_dir, planet_center, planet_radius);
        vec2 atmosphere_hit_info = ray_sphere(ray_origin, ray_dir, planet_center, atmosphere_radius);

        // float dstToSurface         = min(depth, planet_hit_info.x);
        float dst_to_surface = planet_hit_info.x;
        float dst_to_atmosphere = atmosphere_hit_info.x;
        float dst_through_atmosphere = min(atmosphere_hit_info.y, dst_to_surface - dst_to_atmosphere);

        if (dst_through_atmosphere > 0.0) {
            const float epsilon = 0.0001;
            vec3 point_in_atmosphere = ray_origin + ray_dir * (dst_to_atmosphere + epsilon);
            vec3 light = calculate_light(point_in_atmosphere, ray_dir, dst_through_atmosphere - epsilon * 2.0, original_color);

            frag_color = vec4(light, 1.0);
        } else {
            // frag_color = vec4(0, 0, 0, 0);
            frag_color = vec4(ray_dir, 1);
        }
    }
    else
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
}
