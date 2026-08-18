#version 460 core

struct Material{
    uint layer;
    vec4 baseColor; 
};

layout(std430, binding = 2) readonly buffer Materials{
    Material uMaterials[];
};


uniform sampler2DArray uAtlas;

in vec3 vColor;
in vec2 vUV;
in vec3 vNormal;
flat in uint vMaterial;

out vec4 FragColor;


#ifndef ALPHA_CUTOFF
#define ALPHA_CUTOFF 0.5
#endif

void main(){
    Material m = uMaterials[vMaterial];
    vec4 color = texture(uAtlas, vec3(uUV, float(m.layer))) * m.baseColor * vec4(vColor, 1.0);

#ifdef TRANSPARENT_PASS// Blended pass: emit real alpha and let GL blend state do the work.
    FragColor = color;
#else // Opaque pass: alpha-tested cutout, then force fully opaque.
    if (color.a < ALPHA_CUTOFF) {
        discard;
    }
    FragColor = vec4(color.rgb, 1.0);
#endif
}