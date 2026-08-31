/* Sets default values for the render module and contains internal types/functions

*/

#ifndef RENDER_CONFIG_H
#define RENDER_CONFIG_H

#include <stdint.h>
#include "glad/glad.h"

#define AWAIT_GPU_TIMEOUT 1000000ULL 
#define FRAME_COUNT 3

#define DEFAULT_FRAME_COLOR 0.0f, 0.0f, 0.0f, 1.0f

#define MAX_OPTIONS 8


typedef enum EffectTypes{
    FRAGMENT = GL_FRAGMENT_SHADER,
    VERTEX = GL_VERTEX_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
}EffectTypes;


//All code below is deprecated due to change in design pattern
typedef enum EffectOptions{
    TRANSPARENT = 0,
    OPAQUE,
    EFFECT_COUNT
}EffectOptions;

typedef uint32_t SEffect;




//Describes the stages and feature defines that make up one shader program.
//Any stage left NULL is skipped; defines[] are injected after the #version line.
typedef struct ShaderDesc{
    const char* vertexSrc;
    const char* fragmentSrc;
    const char* geometrySrc;
    const char* computeSrc;
    const char* defines[MAX_OPTIONS];
    int defineCount;
}ShaderDesc;

SEffect BuildShader(const ShaderDesc* desc);


#endif