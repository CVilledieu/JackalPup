#ifndef RENDER_SHADER_FILES_H
#define RENDER_SHADER_FILES_H

#include "render/render_config.h"

#define SHADER_FILE_NAME(name) "shaders/" name ".glsl"
#define SHADER_FILE(stageType, name) { \
    .stage = stageType, \
    .fileName = SHADER_FILE_NAME(name) \
}

//A list of preprocessor macros used to control how the StageObject operates.
#define FEATURES(...) (const char *const[]){__VA_ARGS__}



//Transparent pass macros
#define sof_TRANSPARENT "TRANSPARENT_PASS"


//One row per shader source file. Index = ShaderBody. File names are relative to assets/.
typedef struct ShaderFile{
    GLenum stage;
    const char *fileName;
}ShaderFile;


static const ShaderFile shaderFiles[TOTAL_SHADER_BODIES] = {
    [WORLD_SHADER] = SHADER_FILE(GL_VERTEX_SHADER, "world"),
    [SURFACE_SHADER] = SHADER_FILE(GL_FRAGMENT_SHADER, "surface"),
};


//(Stage Src Ref) List of Stages that compose each effect.
static const StageObject SSR_opaque[] = {
    { .srcBody = WORLD_SHADER },
    { .srcBody = SURFACE_SHADER },
};

static const StageObject SSR_transparent[] = {
    { .srcBody = WORLD_SHADER },
    {
        .srcBody = SURFACE_SHADER,
        .options = FEATURES(ShaderFlags(sof_TRANSPARENT)),
        .count = 1
    },
};


//One row per effect. Index = ShaderEffect.
static const ShaderEffectDef shaderEffects[SHADER_EFFECT_COUNT] = {
    [EFFECT_OPAQUE] = {
        .objects = SSR_opaque,
        .objectCount = 2
    },
    [EFFECT_TRANSPARENT] = {
        .objects = SSR_transparent,
        .objectCount = 2
    },
};


#endif