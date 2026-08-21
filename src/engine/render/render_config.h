/* Sets default values for the render module and contains internal types/functions

*/

#ifndef RENDER_CONFIG_H
#define RENDER_CONFIG_H

#include <stdint.h>

#define AWAIT_GPU_TIMEOUT 1000000ULL 
#define FRAME_COUNT 3

#define DEFAULT_FRAME_COLOR 0.0f, 0.0f, 0.0f, 1.0f


//Shader file names
#define WORLD_SHADER_FILE "shaders/world.glsl"
#define SURFACE_SHADER_FILE "shaders/surface.glsl"
#define SHADER_MAX_DEFINES 8


//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"


//Shader file define logic
#define TRANSPARENT_SHADER_DEFINES_COUNT 1
#define TRANSPARENT_SHADER_DEFINES {"#define TRANSPARENT_PASS 1\n"}

#define OPAQUE_SHADER_DEFINES_COUNT 1
#define OPAQUE_SHADER_DEFINES {"#define OPAQUE_PASS 1\n"}



//Shader options
typedef enum ShaderEffects{
    TRANSPARENT = 0,
    OPAQUE,
    SHADER_COUNT
}ShaderEffects;

typedef uint32_t SEffect;

//Describes the stages and feature defines that make up one shader program.
//Any stage left NULL is skipped; defines[] are injected after the #version line.
typedef struct ShaderDesc{
    const char* vertexSrc;
    const char* fragmentSrc;
    const char* geometrySrc;
    const char* computeSrc;
    const char* defines[SHADER_MAX_DEFINES];
    int defineCount;
}ShaderDesc;

SEffect BuildShader(const ShaderDesc* desc);
void DestroyShader(SEffect shader);


#endif