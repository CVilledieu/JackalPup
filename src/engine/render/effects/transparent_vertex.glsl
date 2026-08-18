#version 460 core

struct InstanceXform{
    mat4 model;
    mat4 normal;
};

layout(std140, binding = 0) readonly buffer Camera{
    mat4 projection;
    mat4 view;
};

layout(std430, binding = 0) readonly buffer Transforms{
    InstanceXform uTransforms[];
};

layout(std430, binding = 1) readonly buffer InstanceMaterial{
    uint uInstanceMaterial[];
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;

out vec3 vColor;
out vec2 vUV;
out vec3 vNormal;
flat out uint vMaterial;

void main(){
    uint instance = uint(gl_baseInstance + gl_InstanceID);

    gl_Position = projection * view * uTransforms[instance].model * vec4(aPos, 1.0);

    vColor = a Color;
    vUv = aUV;
    vNormal = uTransforms[instance].normal * aNormal;
    vMaterial = uInstanceMaterial[instance];
    
}
















#extension GL_ARB_shader_draw_parameters : require


layout(std430, binding = 0) readonly buffer Transforms {
    mat4 uTransforms[];
};

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aNormal;

layout(std430, binding = 2) readonly buffer InstanceMaterial {// Per-instance materialId, forwarded (flat) to the fragment stage.
    uint uInstanceMaterial[];
};

out      vec3 vColor;
out      vec2 vUV;
out      vec3 vNormal;    // plumbed for future lighting; currently unused
flat out uint vMaterial;

void main() {
    uint instance = uint(gl_BaseInstance + gl_InstanceID);
    mat4 model    = uTransforms[instance];

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vColor    = aColor;
    vUV       = aUV;
    // Rotation / uniform-scale only. Non-uniform scale would need a dedicated
    // normal matrix (inverse-transpose) rather than mat3(model).
    vNormal   = mat3(model) * aNormal;
    vMaterial = uInstanceMaterial[instance];
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

struct Material {
    uint layer;       // layer index into the shared sampler2DArray atlas
    uint _pad0;
    uint _pad1;
    uint _pad2;
    vec4 baseColor;   // multiplies the sampled texel and the vertex tint
};

layout(std430, binding = 1) readonly buffer Materials {
    Material uMaterials[];
};

uniform sampler2DArray uAtlas; // texture unit 0

in      vec3 vColor;
in      vec2 vUV;
in      vec3 vNormal; // available; intentionally unused for now
flat in uint vMaterial;

out vec4 FragColor;

#ifndef ALPHA_CUTOFF
#define ALPHA_CUTOFF 0.5
#endif

void main() {
    Material mat = uMaterials[vMaterial];
    vec4 color   = texture(uAtlas, vec3(vUV, float(mat.layer))) * mat.baseColor * vec4(vColor, 1.0);

#ifdef TRANSPARENT_PASS// Blended pass: emit real alpha and let GL blend state do the work.
    FragColor = color;
#else // Opaque pass: alpha-tested cutout, then force fully opaque.
    if (color.a < ALPHA_CUTOFF) {
        discard;
    }
    FragColor = vec4(color.rgb, 1.0);
#endif
}
#endif //FRAGMENT_SHADER