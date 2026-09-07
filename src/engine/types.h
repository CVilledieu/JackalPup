//Types that cross module boundries
#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <cglm/cglm.h>

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

typedef struct DrawGroup {
    uint32_t     effectId;       // the ONLY per-group state
    uint32_t     commandCount;
    DrawCommand *commands;       // uploaded to a GL_DRAW_INDIRECT_BUFFER

    // Global instance streams, indexed via baseInstance + gl_InstanceID
    uint32_t     instanceCount;
    InstanceXform *instances;    // or SoA columns as in Option B
    Material      *materials;
} DrawGroup;

#endif