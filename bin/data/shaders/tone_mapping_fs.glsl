#version 330

in vec2 surface_texcoord;

out vec4 fragment_color;

uniform sampler2D image;
uniform float tone_mapping_exposure;
uniform float tone_mapping_gamma;

uniform float tone_mapping_contrast;
uniform float tone_mapping_lifts;
uniform float tone_mapping_flattens;
uniform float tone_mapping_compres;
uniform float tone_mapping_clamp;

vec3 tone_mapping_aces_filmic(vec3 x)
{
  float a = tone_mapping_contrast;
  float b = tone_mapping_lifts;
  float c = tone_mapping_flattens;
  float d = tone_mapping_compres;
  float e = tone_mapping_clamp;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
  vec3 color;

  vec3 texture_sample = texture(image, surface_texcoord).rgb;

  color = texture_sample;

  color = pow(color, vec3(tone_mapping_gamma));

  color = vec3(1.0) - exp(-color * tone_mapping_exposure);

  color = tone_mapping_aces_filmic(color);

  color = pow(color, vec3(1.0 / tone_mapping_gamma));

  fragment_color = vec4(color, 1.0);
}