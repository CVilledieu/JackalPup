#ifndef RENDER_SHADERS_H
#define RENDER_SHADERS_H

#include <stdint.h>

#define SHADER_MAX_DEFINES 8

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

uint32_t BuildShader(const ShaderDesc* desc);
void DestroyShader(uint32_t shader);

#endif