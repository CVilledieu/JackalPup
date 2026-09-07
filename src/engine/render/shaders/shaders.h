#ifndef ENGINE_RENDER_SHADERS_H
#define ENGINE_RENDER_SHADERS_H


//One row per shader source file. Index = ShaderBody. File names are relative to assets/.
typedef struct ShaderFile{
    GLenum stage;
    const char *fileName;
}ShaderFile;

extern ShaderEffectDef shaderEffects;


#endif