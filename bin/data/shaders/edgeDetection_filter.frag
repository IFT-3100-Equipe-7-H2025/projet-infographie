#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D image;
uniform vec2 u_texelSize;
uniform float threshold;

void main() {
    float kernelX[9] = float[](
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    );
    float kernelY[9] = float[](
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    );

    vec2 offsets[9] = vec2[](
        vec2(-1,  1), vec2( 0,  1), vec2( 1,  1),
        vec2(-1,  0), vec2( 0,  0), vec2( 1,  0),
        vec2(-1, -1), vec2( 0, -1), vec2( 1, -1)
    );

    float gx = 0.0;
    float gy = 0.0;
    for (int i = 0; i < 9; ++i) {
        float intensity = dot(texture(image, TexCoord + offsets[i] * u_texelSize).rgb, vec3(0.299, 0.587, 0.114));
        gx += intensity * kernelX[i];
        gy += intensity * kernelY[i];
    }

    float edge = sqrt(gx * gx + gy * gy);
    edge = edge > threshold ? 1.0 : 0.0;
    FragColor = vec4(vec3(edge), 1.0);
}
