#version 330 core

// attributs de sommet
in vec4 position;
in vec4 normal;
in vec2 texcoord; // Add texcoord input

// attributs en sortie
out vec3 surface_position;
out vec3 surface_normal;
out vec3 vViewDir; // Add missing output
out vec2 vUv; // Add missing output

// attributs uniformes
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    // calculer la matrice normale
    mat4 normalMatrix = transpose(inverse(modelViewMatrix));

    // transformation de la normale du sommet dans l'espace de vue
    surface_normal = normalize(vec3(normalMatrix * normal));

    // transformation de la position du sommet dans l'espace de vue
    surface_position = vec3(modelViewMatrix * position);

    // View direction is from position to camera (at origin in eye space)
    vViewDir = normalize(-surface_position);

    // Pass texture coordinates
    vUv = texcoord;

    // transformation de la position du sommet par les matrices de modèle, vue et projection
    gl_Position = projectionMatrix * modelViewMatrix * position;
}
