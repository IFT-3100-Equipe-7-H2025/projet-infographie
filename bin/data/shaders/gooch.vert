#version 330 core

in vec4 position;
in vec3 normal;

out vec3 vNormal;
out vec3 vWorldPos;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix; // inverse‑transpose of modelMatrix’s upper‑left 3×3

void main()
{
    vWorldPos = vec3(modelMatrix * position);
    vNormal = normalize(normalMatrix * normal);

    gl_Position = modelViewProjectionMatrix * position;
}
