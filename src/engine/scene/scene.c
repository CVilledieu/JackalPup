/* DEV NOTE: 
Design: Scene system sends render effect list to renderer to let it know what effects will be used
    Scene then tracks the effect id to pass to render with the drawGroup

*/
#include <cglm/cglm.h>

typedef struct Material {
    /* data */
}Material;

typedef struct InstanceXform{}InstanceXform;

// Mirrors GL's DrawElementsIndirectCommand exactly.
typedef struct DrawCommand {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t baseInstance;   // indexes into the instance SSBO
} DrawCommand;

typedef struct DrawGroup {
    uint16_t     effectId;       // the ONLY per-group state
    uint32_t     commandCount;
    DrawCommand *commands;       // uploaded to a GL_DRAW_INDIRECT_BUFFER

    // Global instance streams, indexed via baseInstance + gl_InstanceID
    uint32_t     instanceCount;
    InstanceXform *instances;    // or SoA columns as in Option B
    Material *materials;
} DrawGroup;

typedef struct Scene{
    DrawGroup *drawGroups;
    uint16_t groupCount;
}Scene;


