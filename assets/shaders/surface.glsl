
struct Material {
    uint layer;       // layer index into the shared sampler2DArray atlas
    uint _pad0;
    uint _pad1;
    uint _pad2;
    vec4 baseColor;   // multiplies the sampled texel and the vertex tint
};

layout(std140, binding = 0) uniform PerFrame{
    mat4 projection;
    mat4 view;
    vec4 uLightDir;   // xyz = direction the light travels; w unused
    vec4 uLightColor; // rgb = color; w = intensity
    vec4 uAmbient;    // rgb = ambient color; w unused
};

layout(std430, binding = 2) readonly buffer Materials{
    Material uMaterials[];
};


uniform sampler2DArray uAtlas; // texture unit 0

in vec3 vColor;
in vec2 vUV;
in vec3 vNormal;
flat in uint vMaterial;

out vec4 FragColor;


#ifndef ALPHA_CUTOFF
#define ALPHA_CUTOFF 0.5
#endif

void main() {
    Material mat = uMaterials[vMaterial];
    vec4 color   = texture(uAtlas, vec3(vUV, float(mat.layer))) * mat.baseColor * vec4(vColor, 1.0);

    // Re-normalize: interpolation across the triangle shortens the normal.
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir.xyz); // vector pointing toward the light
    float diffuse = max(dot(N, L), 0.0);
    vec3 lighting = uAmbient.rgb + uLightColor.rgb * uLightColor.w * diffuse;
    color.rgb *= lighting;

#ifdef TRANSPARENT_PASS// Blended pass: emit real alpha and let GL blend state do the work.
    FragColor = color;
#else // Opaque pass: alpha-tested cutout, then force fully opaque.
    if (color.a < ALPHA_CUTOFF) {
        discard;
    }
    FragColor = vec4(color.rgb, 1.0);
#endif
}





