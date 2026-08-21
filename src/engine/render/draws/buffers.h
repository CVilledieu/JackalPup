#ifndef DRAW_BUFFERS_H
#define DRAW_BUFFERS_H

#include <cglm/cglm.h>
#include "glad/glad.h"
#include "common/logging.h"

//Data updated on Per Frame basis, but not linked to a single entity
typedef struct PerFrame{
    mat4 projection;
    mat4 view;
    vec4 uLightDir;
    vec4 uLightColor;
    vec4 uAmbient;
} PerFrame;

//Render's handle for Per Frame data
typedef struct Globals{
    uint32_t ubo;
    uint8_t* buffer
}Globals;



int Globals_Init(Globals* globals);


#endif