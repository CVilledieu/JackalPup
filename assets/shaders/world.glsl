
struct InstanceXform{
    mat4 model;
    mat4 normal;
};

layout(std140, binding = 0) uniform PerFrame{
    mat4 projection;
    mat4 view;
    vec4 uLightDir;   // xyz = direction the light travels; w unused
    vec4 uLightColor; // rgb = color; w = intensity
    vec4 uAmbient;    // rgb = ambient color; w unused
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

    mat4 model = uTransforms[instance].model;
    vMaterial = uInstanceMaterial[instance];
    vNormal = mat3(uTransforms[instance].normal) * aNormal;
    
    vUV = aUV;
    vColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}



