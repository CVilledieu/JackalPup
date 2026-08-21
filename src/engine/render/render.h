#ifndef RENDER_H
#define RENDER_H
#include "common/types.h"

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


void RenderEngine_init(void);
void RenderEngine_shutdown(void);

int Draw(void);

#endif