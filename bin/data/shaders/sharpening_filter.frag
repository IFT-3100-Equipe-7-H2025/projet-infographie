#version 330
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D image;
uniform vec2 u_texelSize;
uniform float sharpness; // > 1.0 for stronger effect

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-1,  1), vec2( 0,  1), vec2( 1,  1),
        vec2(-1,  0), vec2( 0,  0), vec2( 1,  0),
        vec2(-1, -1), vec2( 0, -1), vec2( 1, -1)
    );

    float kernel[9] = float[](
        0, -1,  0,
       -1,  sharpness, -1,
        0, -1,  0
    );

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        vec2 coord = TexCoord + offsets[i] * u_texelSize;
        color += texture(image, coord).rgb * kernel[i];
    }

    FragColor = vec4(color, 1.0);
}