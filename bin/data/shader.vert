#version 150

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

in vec4 position;

void main() {
    gl_Position = projectionMatrix * modelViewMatrix * position;
}
