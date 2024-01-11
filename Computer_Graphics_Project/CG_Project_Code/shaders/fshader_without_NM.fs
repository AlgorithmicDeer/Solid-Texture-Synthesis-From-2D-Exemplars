#version 330 core

// Input variables
in vec3 n;       // Normal vector
in vec3 e;       // Eye vector
in vec3 l;       // Light vector
in vec2 TexCoord; // Texture coordinates

// Output variable
out vec4 outColor;

// Texture sampler
uniform sampler2D textureSampler;

// Light properties
vec3 Ls = vec3(1.0, 1.0, 1.0); // Specular light color
vec3 Ld = vec3(0.8, 0.7, 0.8); // Diffuse light color
vec3 La = vec3(0.6, 0.3, 0.4); // Ambient light color

vec3 ks = vec3(1.0, 1.0, 1.0); // Specular reflection constant
vec3 kd = vec3(0.5, 0.6, 0.4); // Diffuse reflection constant
vec3 ka = vec3(1.0, 1.0, 1.0); // Ambient reflection constant

float spec_exp = 7; // Specular exponent

void main(void) {
    // Ambient component
     vec3 Ia = ka * La;

    // Diffuse component
    vec3 Id = kd * max(dot(n, l) * Ld, 0.0);

    // Specular component
    vec3 v = normalize(e);
    vec3 h = normalize(l + v);
    vec3 Is = ks * Ls * max(pow(dot(h, n), spec_exp), 0);

    // Final color calculation
    vec3 fColor = Ia + Id + Is;
    // Combine ambient, diffuse, and specular components with material texture
    outColor = 0.4 * vec4(fColor, 1.0) + 0.8 * texture(textureSampler, TexCoord);
    // Note: Adjust the weights (0.4 and 0.8) based on desired material properties.
}
