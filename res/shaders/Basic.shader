#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 mvp;

out vec3 fragPos;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    fragPos = aPos;
}

#shader fragment
#version 330 core

in vec3 fragPos;

out vec4 FragColor;

uniform vec4 ourColor;
uniform vec3 lightPos;

void main()
{

    // Calculate distance between the fragment and the light source
    float distance = length(lightPos - fragPos);

    // Compute light intensity based on distance (simple attenuation)
    float attenuation = 1.0 / (distance * distance); // Inverse square law

    // Clamp attenuation to avoid overly bright results
    attenuation = clamp(attenuation, 0.0, 1.0);

    vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light

    // Final color
    vec3 result = ourColor.xyz * lightColor * attenuation;
    FragColor = vec4(result, 1.0);
}
