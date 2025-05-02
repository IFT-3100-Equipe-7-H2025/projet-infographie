#version 330

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 surface_texcoord;

void main()
{
  gl_Position = modelViewProjectionMatrix * position;

  surface_texcoord = texcoord;
}
