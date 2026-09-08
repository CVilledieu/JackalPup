#ifndef DRAW_BUFFERS_H
#define DRAW_BUFFERS_H

#include "engine/types.h"
#include "glad/glad.h"
#include "common/logging.h"



//Render's handle for Per Frame data
typedef struct Globals{
    uint32_t ubo;
    uint8_t* buffer;
}Globals;



int Globals_Init(Globals* globals);


#endif