#version 330 core
in  vec3 vNormal;
in  vec3 vWorldPos;

out vec4 fragColor;

/* ------------- user‑supplied uniforms ------------- */
uniform vec3  uSurfaceColor;            // intrinsic object colour
uniform vec3  uLightPos;                // point light position (world space)
uniform vec3  uWarmColor   = vec3(1.0, 0.6, 0.0);   // warm tint  (orange)
uniform vec3  uCoolColor   = vec3(0.0, 0.0, 1.0);   // cool tint  (blue)
uniform float uAlpha       = 0.25;      // warm mix factor
uniform float uBeta        = 0.25;      // cool mix factor

/* ------------- Gooch shading ---------------------- */
void main()
{
    vec3  N  = normalize(vNormal);
    vec3  L  = normalize(uLightPos - vWorldPos);

    float NdotL = dot(N, L);            // [-1, 1]
    float k     = (NdotL + 1.0) * 0.5;  // remap to [0, 1]

    /* warm & cool terms per original paper -------------------------------- */
    vec3 kCool = uCoolColor + uBeta  * uSurfaceColor;
    vec3 kWarm = uWarmColor + uAlpha * uSurfaceColor;

    /* interpolate between them */
    vec3 colour = mix(kCool, kWarm, k);

    fragColor = vec4(colour, 1.0);
}
