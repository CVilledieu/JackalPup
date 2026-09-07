#ifndef RENDER_SHADER_CONFIG_H
#define RENDER_SHADER_CONFIG_H

#include <stdint.h>
#include "glad/glad.h"
#include "common/util.h"
#include "engine/render/shaders/shaders.h"

//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"

//Build a "#define OP\n" line from a shader flag string
#define ShaderFlags(op) "#define " op "\n"
//Build a "#define OP VAL\n" line from a shader flag string and a value
#define ShaderConfig(op, val) "#define " op " " TOSTRING(val) "\n"


//One row per shader source file. Index = ShaderBody. File names are relative to assets/.
typedef struct ShaderFile{
    GLenum stage;
    const char *fileName;
}ShaderFile;

//Index of a single shader source file/stage in the shader file table
typedef enum ShaderBody{
    WORLD_SHADER,
    SURFACE_SHADER,
    TOTAL_SHADER_BODIES
}ShaderBody;


//One compiled stage of an effect: a source body plus its feature defines
typedef struct StageObject{
    ShaderBody srcBody;
    const char *const *options;
    uint8_t count;
}StageObject;

//The set of stages that compose one effect
typedef struct ShaderEffectDef{
    const StageObject *objects;
    uint8_t objectCount;
}ShaderEffectDef;



//Load every shader source body from disk once. Returns 1 on success, 0 on failure.
uint8_t InitShaderData(void);
//Release the source bodies loaded by InitShaderData; safe to call after all effects are built.
void FreeShaderData(void);
//Compile and link the requested effect, returning its program ID (0 on failure).
uint32_t CreateDrawEffect(ShaderEffect effect);
//Delete a program previously returned by CreateDrawEffect.
void DestroyEffect(uint32_t program);



extern const ShaderFile shaderFiles[TOTAL_SHADER_BODIES];
extern const ShaderEffectDef shaderEffects[SHADER_EFFECT_COUNT];



#endif