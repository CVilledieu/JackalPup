#ifndef RENDER_CONFIG_H
#define RENDER_CONFIG_H

#include <stdint.h>
#include "glad/glad.h"
#include "common/util.h"

#define AWAIT_GPU_TIMEOUT 1000000ULL 
#define FRAME_COUNT 3

#define DEFAULT_FRAME_COLOR 0.0f, 0.0f, 0.0f, 1.0f

//Shader version data included on all shaders
#define SHADER_VERSION "#version 460 core \n"

//Build a "#define OP\n" line from a shader flag string
#define ShaderFlags(op) "#define " op "\n"
//Build a "#define OP VAL\n" line from a shader flag string and a value
#define ShaderConfig(op, val) "#define " op " " TOSTRING(val) "\n"


//Index of a single shader source file/stage in the shader file table
typedef enum ShaderBody{
    WORLD_SHADER,
    SURFACE_SHADER,
    TOTAL_SHADER_BODIES
}ShaderBody;

//Index of a linked shader effect (program) in the effect table
typedef enum ShaderEffect{
    EFFECT_OPAQUE,
    EFFECT_TRANSPARENT,
    SHADER_EFFECT_COUNT
}ShaderEffect;

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


// --- Shader module (internal to the render module) ---

//Load every shader source body from disk once. Returns 1 on success, 0 on failure.
uint8_t InitShaderData(void);
//Release the source bodies loaded by InitShaderData; safe to call after all effects are built.
void FreeShaderData(void);
//Compile and link the requested effect, returning its program ID (0 on failure).
uint32_t CreateDrawEffect(ShaderEffect effect);
//Delete a program previously returned by CreateDrawEffect.
void DestroyEffect(uint32_t program);


#endif