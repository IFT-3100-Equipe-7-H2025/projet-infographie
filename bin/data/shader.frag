#version 150

uniform vec4 inputColor;  // RGBA color from C++

out vec4 outputColor;

void main() {
    outputColor = inputColor;
}
