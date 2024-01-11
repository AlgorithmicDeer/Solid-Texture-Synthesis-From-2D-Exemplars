#version 330 core

in vec3 n; // Normal Vector
in vec3 e; // Eye vector
in vec3 l; // Light vector
in vec2 TexCoord; // Texture coordinates
in vec3 tangent;
in vec3 bitangent;
out vec4 outColor;

uniform sampler2D textureSampler; // color texture
uniform sampler2D normalMap; // normal map

// $---------------------------------------------------------------
// uniform sampler2D depthMap;
// const float height_scale = 0.1;
// // Function to perform Steep Parallax Mapping
// vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir) {
//     const float minLayers = 8.0;
//     const float maxLayers = 32.0;
//     float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

//     // Calculate the size of each layer
//     float layerDepth = 1.0 / numLayers;
//     // Depth of the current layer
//     float currentLayerDepth = 0.0;
//     // Texture coordinate offset per layer (from vector P)
//     vec2 P = viewDir.xy * height_scale;
//     vec2 deltaTexCoords = P / numLayers;

//     // Get initial values
//     vec2 currentTexCoords = texCoords;
//     float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

//     // Traverse depth layers
//     while (currentLayerDepth < currentDepthMapValue) {
//         // Shift texture coordinates along the direction of P
//         currentTexCoords -= deltaTexCoords;
//         // Get depthmap value at current texture coordinates
//         currentDepthMapValue = texture(depthMap, currentTexCoords).r;
//         // Get depth of the next layer
//         currentLayerDepth += layerDepth;
//     }

//     return currentTexCoords;
// }
// $--------------------------------------------------------------


// Light properties
vec3 Ls = vec3(1.0, 1.0, 1.0); // Specular light color
vec3 Ld = vec3(0.8, 0.7, 0.8); // Diffuse light color
vec3 La = vec3(0.6, 0.3, 0.4); // Ambient light color

vec3 ks = vec3(1.0, 1.0, 1.0); // Specular reflection constant
vec3 kd = vec3(0.5, 0.6, 0.4); // Diffuse reflection constant
vec3 ka = vec3(1.0, 1.0, 1.0); // Ambient reflection constant

float spec_exp = 7; // Specular exponent

void main(void) {
    // Construct the TBN matrix
    // mat3 TBN
    mat3 TBN = transpose(mat3(tangent, bitangent, normalize(n)));

    // Load and transform the normal from the normal map into tangent space
    vec3 normalMapColor = texture(normalMap, TexCoord).xyz * 2.0 - 1.0;
    vec3 normal = normalize(normalMapColor);
    // To-Do: Transform the light vector and eye vector into tangent space
    vec3 tLight = normalize(TBN * l);
    vec3 tView = normalize(TBN * e);

// $------------------------------------------------------------------------------------
    //  // Perform Steep Parallax Mapping to get the final texture coordinates
    // vec2 parallaxCoords = SteepParallaxMapping(TexCoord, tView);
// $------------------------------------------------------------------------------------

    // Ambient component
    vec3 Ia = ka * La;
    // Diffuse component
    vec3 Id = kd * max(dot(normal, tLight), 0.0) * Ld;
    // Specular component
    // vec3 r = normalize(reflect(-tLight, normal)); // reflect vector
    vec3 h = normalize(tLight + tView);
    vec3 Is = ks * Ls * pow(max(dot(h, normal), 0.0), spec_exp);
    // Combine the components
    vec3 fColor = Ia + Id + Is;
    // // Final color
    outColor = 0.4 * vec4(fColor, 1.0) + 0.8 *texture(textureSampler, TexCoord);
    
// -------------------------------------------------------------------------------------------------
   // Final color with texture mapping using Steep Parallax Mapping
//    outColor = 0.2 * vec4(fColor, 1.0) + 0.8 * texture(textureSampler, parallaxCoords);
// -------------------------------------------------------------------------------------------------
}
