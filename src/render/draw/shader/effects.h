#ifndef DRAW_SHADER_EFFECTS_H
#define DRAW_SHADER_EFFECTS_H

typedef uint32_t Effect;


//Shader Effect Recipe
typedef struct SER{
    const char* body;
    const char* options;
    GLenum stageType;
}SER;



#endif