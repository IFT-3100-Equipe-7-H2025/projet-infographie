#version 330 core

layout(location = 0) in vec3 position;
out vec3 TexDir;

uniform mat4 modelViewProjection;

void main()
{
    TexDir = position; // must vary across vertices
    gl_Position = modelViewProjection * vec4(position, 1.0);
}