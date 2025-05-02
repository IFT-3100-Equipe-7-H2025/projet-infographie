#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D image;
uniform float negateAmount;

void main() {
    vec3 color = texture(image, TexCoord).rgb;
    vec3 negated = vec3(1.0) - color;
    vec3 finalColor = mix(color, negated, negateAmount);
    FragColor = vec4(finalColor, 1.0);
}

