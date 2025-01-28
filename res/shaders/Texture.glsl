#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * pos;
    TexCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 TexCoord;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, TexCoord);
    color = texColor;
}
