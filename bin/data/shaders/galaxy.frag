#version 330 core

in vec3 TexDir;
out vec4 FragColor;

uniform float u_time;

// ---------- Simple noise / fbm ----------
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < 5; ++i) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

// ---------- Main shader ----------
void main()
{
    vec3 dir = normalize(TexDir);
    vec2 uv = dir.xy * 1.5;//plus petit multiplicateur, plus grosse galaxy

    float t = u_time * 0.05;//reduce time 0.1 to  0.02 equal 5x

    float dist = length(uv);
    float angle = atan(uv.y, uv.x);
    float radius = dist;

    // Spiral twist
    vec2 swirl = vec2(
        cos(angle + radius * 4.0),//tightness of the spiral arms
        sin(angle + radius * 4.0)
    ) * radius;

    float pattern = fbm(swirl * 3.0 - t);

    vec3 galaxyColor = mix(
    vec3(0.02, 0.02, 0.08),   // deep navy blue
    vec3(0.4, 0.6, 1.0),      // soft electric blue
    pattern
	);


    float falloff = smoothstep(1.0, 0.0, dist);//1.0 to 1.5 to prevent edge clipping
    galaxyColor *= falloff;

    //Star field
    float starNoise = noise(uv * 150.0 + t * 5.0);
    float stars = step(0.996, fract(starNoise));
    vec3 starColor = vec3(stars);

    //Background color (tint for dark space)
    vec3 background = vec3(0.01, 0.02, 0.06); // deep space blue

    // Combine: background + galaxy + stars
    vec3 finalColor = background + galaxyColor + starColor;

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
