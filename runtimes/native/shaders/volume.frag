#version 330 core

void main() {
    vec3 originalColor   = texture2D(colorBuffer, gl_FragCoord.xy / resolution.xy).xyz;
    // float depthNonLinear = texture2D(depthBuffer, gl_FragCoord.xy / resolution.xy).r;
    // float depth          = linearizeDepth(depthNonLinear, 0.1, 1000.0);

    vec3 rayOrigin    = cameraPosition;
    vec3 rayDirection = normalize(vertexWorldPosition.xyz - cameraPosition);

    vec2 planetHitInfo     = raySphere(rayOrigin, rayDirection, planetCenter, planetRadius);
    vec2 atmosphereHitInfo = raySphere(rayOrigin, rayDirection, planetCenter, atmosphereRadius);

    // float dstToSurface         = min(depth, planetHitInfo.x);
    float dstToSurface         = planetHitInfo.x;
    float dstToAtmosphere      = atmosphereHitInfo.x;
    float dstThroughAtmosphere = min(atmosphereHitInfo.y, dstToSurface - dstToAtmosphere);

    if (dstThroughAtmosphere > 0.0) {
        const float epsilon    = 0.0001;
        vec3 pointInAtmosphere = rayOrigin + rayDirection * (dstToAtmosphere + epsilon);
        vec3 light             = calculateLight(pointInAtmosphere, rayDirection, dstThroughAtmosphere - epsilon * 2.0, originalColor);
    
        gl_FragColor = vec4(light, 1.0);
    } else {
        gl_FragColor = vec4(0, 0, 0, 0);
    }       
}
