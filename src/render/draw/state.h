#ifndef RENDER_DRAW_H
#define RENDER_DRAW_H

#include "glad/glad.h"

typedef struct DrawState{
    uint8_t frame;
    GLsync frames[(size_t)BUFFER_SOCKETS];
    uint32_t effects[(size_t)TOTAL_SHADER_EFFECTS];
}DrawState;


int DrawState_init(DrawState* ds);


#endif