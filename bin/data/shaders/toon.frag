#version 330 core

in vec3 surface_position;
in vec3 surface_normal;
in vec3 vViewDir;
in vec2 vUv;

uniform vec3 lightPosEye = vec3(0.0, 0.0, 5.0); // eye-space light
uniform vec3 baseColor = vec3(0.8, 0.4, 0.1); // brownish
uniform int bandCount = 3; // # of diffuse bands
uniform float rimWidth = 0.3; // rim thickness
uniform float rimPower = 2.0; // rim falloff

out vec4 fragColor;

void main()
{
    // Properly normalize vectors
    vec3 N = normalize(surface_normal);

    // calculer la direction de la surface vers la lumière (l)
    // Use lightPosEye or light_position depending on which is set in Node.cpp
    vec3 L = normalize(lightPosEye - surface_position);
    vec3 V = normalize(vViewDir); // Use the passed view direction

    // Basic diffuse lighting
    float NdotL = max(dot(N, L), 0.0);

    // Ensure we don't divide by zero
    int actualBandCount = max(bandCount, 2);

    // Quantize into bands - adjusted calculation
    float band = floor(NdotL * actualBandCount) / float(actualBandCount - 1);

    // Add ambient light so shadows aren't completely black
    band = max(band, 0.1);

    // Rim light calculation
    float rim = pow(1.0 - max(dot(N, V), 0.0), rimPower);
    rim = smoothstep(rimWidth, 1.0, rim);

    // Final color calculation
    vec3 colour = baseColor * band + rim * vec3(1.0);

    // Output the final color
    fragColor = vec4(colour, 1.0);

    // Debug output - uncomment to help diagnose problems
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Pure red for visibility testing
}
