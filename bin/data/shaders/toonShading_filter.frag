#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D image;
uniform int toonSteps;

void main() {
    vec3 color = texture(image, TexCoord).rgb;

    // Apply posterization per channel (hard quantization)
    vec3 quantizedColor = floor(color * float(toonSteps)) / float(toonSteps);

    FragColor = vec4(quantizedColor, 1.0);
}