//Types that cross module boundries
#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <cglm/cglm.h>

//Index of a linked shader effect (program) in the effect table
typedef enum ShaderEffect : uint32_t{
    EFFECT_OPAQUE,
    EFFECT_TRANSPARENT,
    SHADER_EFFECT_COUNT
}ShaderEffect;



//Data updated on Per Frame basis, but not linked to a single entity
typedef struct PerFrame{
    mat4 projection;
    mat4 view;
    vec4 uLightDir;
    vec4 uLightColor;
    vec4 uAmbient;
} PerFrame;



// Mirrors GL's DrawElementsIndirectCommand exactly.
typedef struct DrawCommand {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t baseInstance;   // indexes into the instance SSBO
} DrawCommand;

typedef struct InstanceXform{
    mat4 model;
    mat4 normal;
}InstanceXform;

typedef struct Material{
    uint8_t layer;
    uint8_t _pad0;
    uint8_t _pad1;
    uint8_t _pad2;
    vec4 baseColor;
}Material;




typedef struct DrawGroup {
    ShaderEffect     effectId;       // the ONLY per-group state
    uint32_t     commandCount;
    DrawCommand *commands;       // uploaded to a GL_DRAW_INDIRECT_BUFFER

    // Global instance streams, indexed via baseInstance + gl_InstanceID
    uint32_t     instanceCount;
    InstanceXform *instances;    // or SoA columns as in Option B
    Material      *materials;
} DrawGroup;

#endif